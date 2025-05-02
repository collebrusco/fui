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


