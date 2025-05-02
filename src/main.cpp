#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
#include "UI.h"
LOG_MODULE(main);


struct TestBox : UIelement {
    bool hovered = false;
    bool clicked = false;

    TestBox(glm::vec2 size_) {
        size = size_;
    }

    void onMouseHoverEnter(Mouse const&) override final {
        hovered = true;
    }

    void onMouseHoverExit(Mouse const&) override final {
        hovered = false;
    }

    void onMousePress(Mouse const&) override final {
        clicked = true;
    }

    void onMouseRelease(Mouse const&) override final {
        clicked = false;
    }

    void onDraw() const override final {
        glm::vec3 color;
        if (clicked)      color = {0.f, 1.f, 0.f};
        else if (hovered) color = {1.f, 1.f, 0.f};
        else              color = {1.f, 0.f, 0.f};

        glScissor(bbox.min().x, bbox.min().y, bbox.size().x, bbox.size().y);
        glClearColor(color.r, color.g, color.b, 1.f);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }
};

int main() {
    gl.init();
    window.create("hello window", 1280, 720);

    UI ui;

    for (int i = 0; i < UI::PIN_LAST; ++i) {
        auto& root = ui.roots[i];

        TestBox* test = ui.alloc.push<TestBox>(glm::vec2(40.f, 20.f));

        test->offset = glm::vec2(0.f, 0.f);
        test->parent = &root;
        root.children.push_back(test);
    }

    while (!window.should_close()) {
        if (window.keyboard[GLFW_KEY_ESCAPE].down) window.close();
        glClearColor(0.f,0.f,0.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        SLOW_LOG_INF(20, "mpos %.0f, %.0f", window.mouse.pos.x, window.mouse.pos.y);

        ui.tick(1.f / 60.f, window.mouse);
        ui.draw();

        window.update();
    }

    gl.destroy();
    return 0;
}

