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
    UIbbox operator+(UIbbox const& other) const;
    UIbbox& operator+=(UIbbox const& other);
};

struct UIelement {
    UIbbox bbox;
    virtual UIelement* children() = 0;
    virtual size_t nchildren() const = 0;
};

struct UI {
    UIelement* root;
};


#endif /* UI_H */
