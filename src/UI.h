/** 
 * UI.h 
 * created 04/05/25 by frank collebrusco
 */
#ifndef UI_H
#define UI_H
#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/glm.h>
#include <flgl/allocators.h>
#include <vector>

struct UIbbox {
    bool inside(glm::vec2 mpos) const;
    void merge(UIbbox const& other);
    bool isect(UIbbox const& other);
    bool contains(UIbbox const& other);
    bool degen() const;
    glm::vec2 size() const;
    glm::vec2 middle() const;
    glm::vec2 const& min() const;
    glm::vec2 const& max() const;
    static UIbbox from_merge(UIbbox const& a, UIbbox const& b);
    static UIbbox from_isect(UIbbox const& a, UIbbox const& b);
    static UIbbox from_minmax(glm::vec2 min, glm::vec2 max);
    static UIbbox from_minsize(glm::vec2 min, glm::vec2 size);
    static inline UIbbox null() { return UIbbox(); }
    UIbbox() : _min(1e100f), _max(-1e100f) {}
private:
    glm::vec2 _min, _max;
    UIbbox(glm::vec2 m, glm::vec2 M) : _min(m), _max(M) {}
};


struct UIelement {
    virtual ~UIelement() = default;

    UIelement* parent = nullptr;
    glm::vec2 offset = glm::vec2(0.f);

    glm::vec2 size;
    UIbbox bbox;
    UIbbox subtree_bbox;

    std::vector<UIelement*> children;

    // Must be implemented by derived elements
    virtual void onUpdate(float dt);
    virtual void onMousePress(Mouse const& mouse);
    virtual void onMouseRelease(Mouse const& mouse);
    virtual void onMouseHoverEnter(Mouse const& mouse);
    virtual void onMouseHoverExit(Mouse const& mouse);
    virtual void onDraw() const;

    // Utilities (non-virtual)
    glm::vec2 get_absolute_pos() const;
    UIelement* hitTest(glm::vec2 mpos);
    void updateSubtreeBBox();
};



typedef abstract_linear_objpool<UIelement, 8192> uialloc_t;

struct UI {
    uialloc_t alloc;

    enum pin_e {
        PIN_TOPLEFT,
        PIN_TOPCENTER,
        PIN_TOPRIGHT,
        PIN_CENTERLEFT,
        PIN_CENTERCENTER,
        PIN_CENTERRIGHT,
        PIN_BOTLEFT,
        PIN_BOTCENTER,
        PIN_BOTRIGHT,
        PIN_LAST
    };

    struct UIRootElement : public UIelement {
        UIRootElement(pin_e pin);
        virtual void onUpdate(float dt) override final;
        inline pin_e getpin() const {return pin;}
    private:
        const pin_e pin;
    };

    UIRootElement roots[PIN_LAST];

    UI();

    // Frame lifecycle
    void tick(float dt, Mouse const& mouse);
    void draw() const;

private:
    UIelement* hovered = nullptr;
    UIelement* held = nullptr;

    UIelement* hitTest(glm::vec2 mpos) const;
};


#endif /* UI_H */
