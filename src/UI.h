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

typedef heap_bumpalloc_t<4096> uialloc_t;

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
    static inline UIbbox null() {return UIbbox();}
    UIbbox() : _min(1e100), _max(-1e100) {}
private:
    glm::vec2 _min, _max;
    UIbbox(glm::vec2 m, glm::vec2 M) : _min(m), _max(M) {}
};

struct UIelement {
    virtual ~UIelement() = default;
    UIbbox bbox;
    UIbbox subtree_bbox;
    UIelement* parent;
    glm::vec2 offset;
    std::vector<UIelement*> children;
    virtual void onUpdate(const float dt);
    virtual void onMousePress(Mouse const& mouse);
    virtual void onMouseRelease(Mouse const& mouse);
    virtual void onMouseHoverEnter(Mouse const& mouse);
    virtual void onMouseHoverExit(Mouse const& mouse);
    virtual void draw() const;
};

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
    UIelement* roots[PIN_LAST];
    UI();
};


#endif /* UI_H */
