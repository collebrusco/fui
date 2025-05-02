/** 
 * UI.h 
 * created 04/05/25 by frank collebrusco
 */
#ifndef UI_H
#define UI_H
#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/glm.h>

class BumpAllocator {
public:
    BumpAllocator(size_t const n) : _size(n), pt(0) {buf = (char*)malloc(n); assert(buf);}
    ~BumpAllocator() {free(buf);}
    char* malloc(size_t n) {char* p = buf + pt; pt += n; if (pt > size()) return 0; return p;}
    void clear() {pt = 0;}
    size_t size() const {return _size;}
    size_t left() const {return _size - pt;}
private:
    size_t const _size;
    size_t pt;
    char* buf;
};

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

struct UIanchor {
    enum vertical {TOP = 0, VCENTER, BOTTOM}     V : 4;
    enum horizontal {LEFT = 0, HCENTER, RIGHT}   H : 4;
    UIanchor() : V(VCENTER), H(HCENTER) {}
    UIanchor(vertical v, horizontal h) : V(v), H(h) {}
};

struct UIlayout {
    UIanchor anchor;
    glm::vec2 pivot, offset, size;
};

class UIelement {
public:     /** basics and accessors */
    virtual ~UIelement() = default;
    UIbbox const& readBB() const {return bbox;}
    UIlayout const& read_layout() const {return layout;}
    bool dirty() const {return modif;}
    UIelement* parent{0};
    std::vector<UIelement*> children;
    
    void compute_layout(UIbbox parentBB, bool force_update = false) {
        if (!(dirty() || force_update)) return;
        modif = false;
    
        glm::vec2 parentSize = parentBB.size();
        glm::vec2 anchorPos;
    
        // horizontal anchor
        switch (layout.anchor.H) {
            case UIanchor::LEFT:
                anchorPos.x = parentBB.min().x;
                break;
            case UIanchor::HCENTER:
                anchorPos.x = parentBB.min().x + parentSize.x / 2.f;
                break;
            case UIanchor::RIGHT:
                anchorPos.x = parentBB.max().x;
                break;
        }
    
        // vertical anchor
        switch (layout.anchor.V) {
            case UIanchor::TOP:
                anchorPos.y = parentBB.min().y;
                break;
            case UIanchor::VCENTER:
                anchorPos.y = parentBB.min().y + parentSize.y / 2.f;
                break;
            case UIanchor::BOTTOM:
                anchorPos.y = parentBB.max().y;
                break;
        }
    
        glm::vec2 pivotOffset = layout.pivot * layout.size;
        glm::vec2 finalMin = anchorPos - pivotOffset + layout.offset;
        glm::vec2 finalMax = finalMin + layout.size;
    
        bbox = UIbbox::from_minmax(finalMin, finalMax);
    
        for (UIelement* child : children) {
            child->compute_layout(bbox);
        }
    }
    
    
protected:  /** subclass sandbox for changing layout data */
    UIelement() {modif = true;}
    // UIbbox& getBB() {modif = true; return bbox;}
    UIlayout& get_layout() {modif = true; return layout;}

private:    /** data members */
    bool modif;
    UIbbox bbox;
    UIlayout layout;
};

struct UI {
    UIelement* root;
    void layout(bool force_update = false) {
        if (root) {
            root->compute_layout(UIbbox::from_minmax(glm::vec2(0.), static_cast<glm::vec2>(window.frame)), force_update);
        }
    }
    void tick() {
        if (window.new_size()) {
            layout(true);
        }
    }
};


#endif /* UI_H */
