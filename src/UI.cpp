#include "UI.h"

bool UIbbox::inside(glm::vec2 mpos) const {
    return mpos.x >= _min.x && mpos.x <= _max.x &&
           mpos.y >= _min.y && mpos.y <= _max.y;
}

void UIbbox::merge(UIbbox const& other) {
    if (other.degen()) return;
    if (degen()) {
        _min = other._min;
        _max = other._max;
    } else {
        _min = glm::min(_min, other._min);
        _max = glm::max(_max, other._max);
    }
}

bool UIbbox::isect(UIbbox const& other) {
    return !(other._max.x < _min.x || other._min.x > _max.x ||
             other._max.y < _min.y || other._min.y > _max.y);
}

bool UIbbox::contains(UIbbox const& other) {
    return _min.x <= other._min.x && _max.x >= other._max.x &&
           _min.y <= other._min.y && _max.y >= other._max.y;
}

bool UIbbox::degen() const {
    return _min.x > _max.x || _min.y > _max.y;
}

glm::vec2 UIbbox::size() const {
    return _max - _min;
}

glm::vec2 UIbbox::middle() const {
    return (_min + _max) * 0.5f;
}

glm::vec2 const& UIbbox::min() const {
    return _min;
}

glm::vec2 const& UIbbox::max() const {
    return _max;
}

UIbbox UIbbox::from_merge(UIbbox const& a, UIbbox const& b) {
    if (a.degen()) return b;
    if (b.degen()) return a;
    return UIbbox(glm::min(a._min, b._min), glm::max(a._max, b._max));
}

UIbbox UIbbox::from_isect(UIbbox const& a, UIbbox const& b) {
    glm::vec2 new_min = glm::max(a._min, b._min);
    glm::vec2 new_max = glm::min(a._max, b._max);
    return UIbbox(new_min, new_max);
}

UIbbox UIbbox::from_minmax(glm::vec2 min, glm::vec2 max) {
    return UIbbox(min, max);
}

UIbbox UIbbox::from_minsize(glm::vec2 min, glm::vec2 size) {
    return UIbbox(min, min + size);
}

void UIelement::onUpdate(float) {}
void UIelement::onMousePress(Mouse const&) {}
void UIelement::onMouseRelease(Mouse const&) {}
void UIelement::onMouseHoverEnter(Mouse const&) {}
void UIelement::onMouseHoverExit(Mouse const&) {}
void UIelement::onDraw() const {
}

glm::vec2 UIelement::get_absolute_pos() const {
    return parent ? parent->get_absolute_pos() + offset : offset;
}

UIelement* UIelement::hitTest(glm::vec2 mpos) {
    for (UIelement* child : children) {
        if (UIelement* hit = child->hitTest(mpos)) return hit;
    }
    if (bbox.inside(mpos)) return this;
    return 0;
}

void UIelement::updateSubtreeBBox() {
    glm::vec2 abs_pos = get_absolute_pos();
    glm::vec2 half = size * 0.5f;
    bbox = UIbbox::from_minmax(abs_pos - half, abs_pos + half);

    subtree_bbox = bbox;
    for (UIelement* c : children) {
        c->updateSubtreeBBox();
        subtree_bbox.merge(c->subtree_bbox);
    }
}

UI::UI()
    : roots{
        UIRootElement(PIN_TOPLEFT),
        UIRootElement(PIN_TOPCENTER),
        UIRootElement(PIN_TOPRIGHT),
        UIRootElement(PIN_CENTERLEFT),
        UIRootElement(PIN_CENTERCENTER),
        UIRootElement(PIN_CENTERRIGHT),
        UIRootElement(PIN_BOTLEFT),
        UIRootElement(PIN_BOTCENTER),
        UIRootElement(PIN_BOTRIGHT)
    } {}

void UI::tick(float dt, Mouse const& mouse) {
    for (UIRootElement& r : roots) r.onUpdate(dt);

    for (UIRootElement& r : roots) r.updateSubtreeBBox();

    glm::vec2 flipped_mouse_pos = { mouse.pos.x, window.frame.y - mouse.pos.y };
    UIelement* now_hovered = hitTest(flipped_mouse_pos);
    if (hovered != now_hovered) {
        if (hovered) hovered->onMouseHoverExit(mouse);
        if (now_hovered) now_hovered->onMouseHoverEnter(mouse);
        hovered = now_hovered;
    }

    if (mouse.left().pressed && hovered) hovered->onMousePress(mouse);
    if (mouse.left().released && hovered) hovered->onMouseRelease(mouse);

    for (UIRootElement& r : roots) {
        std::vector<UIelement*> stack{&r};
        while (!stack.empty()) {
            UIelement* e = stack.back(); stack.pop_back();
            e->onUpdate(dt);
            for (auto* c : e->children) stack.push_back(c);
        }
    }
}

void UI::draw() const {
    for (const UIRootElement& r : roots) {
        std::vector<UIelement const*> stack{&r};
        while (!stack.empty()) {
            UIelement const* e = stack.back(); stack.pop_back();
            e->onDraw();
            for (auto* c : e->children) stack.push_back(c);
        }
    }
}

UIelement* UI::hitTest(glm::vec2 mpos) const {
    for (const UIRootElement& root : roots) {
        if (UIelement* hit = const_cast<UIRootElement&>(root).hitTest(mpos)) return hit;
    }
    return nullptr;
}

UI::UIRootElement::UIRootElement(pin_e p) : pin(p) {
    assert(p >= 0 && p < PIN_LAST);
    parent = nullptr;
    offset = glm::vec2(0.f);
    bbox = UIbbox::from_minmax(offset, offset);
    subtree_bbox = bbox;
}

void UI::UIRootElement::onUpdate(float) {
    glm::vec2 win = glm::vec2(window.frame);
    switch (pin) {
        case PIN_TOPLEFT:       offset = {0.f, win.y}; break;
        case PIN_TOPCENTER:     offset = {win.x * 0.5f, win.y}; break;
        case PIN_TOPRIGHT:      offset = {win.x, win.y}; break;
        case PIN_CENTERLEFT:    offset = {0.f, win.y * 0.5f}; break;
        case PIN_CENTERCENTER:  offset = {win.x * 0.5f, win.y * 0.5f}; break;
        case PIN_CENTERRIGHT:   offset = {win.x, win.y * 0.5f}; break;
        case PIN_BOTLEFT:       offset = {0.f, 0.f}; break;
        case PIN_BOTCENTER:     offset = {win.x * 0.5f, 0.f}; break;
        case PIN_BOTRIGHT:      offset = {win.x, 0.f}; break;
        default: break;
    }
}


