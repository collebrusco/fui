#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
#include "UI.h"
LOG_MODULE(main);

static glm::vec2 touv(glm::ivec2 pix) {
    return ((glm::vec2)pix) / 1024.f;
}

const UIbbox uibb_inventory  = UIbbox::from_minmax(touv({0,858}), touv({175,1024}));

const UIbbox uibb_heart = UIbbox::from_minmax(touv({0,849}), touv({9,858}));

const UIbbox uibb_food = UIbbox::from_minmax(touv({0,839}), touv({9,848}));


const UIbbox uibb_halfheart = UIbbox::from_minmax(touv({9,849}), touv({18,858}));

const UIbbox uibb_halffood = UIbbox::from_minmax(touv({9,839}), touv({18,848}));

const UIbbox uibb_hotbar = UIbbox::from_minsize(touv({176,1002}), touv({182,22}));

const UIbbox uibb_hotbar_select = UIbbox::from_minsize(touv({176,798}), touv({24,24}));

const UIbbox uibb_butpress = UIbbox::from_minsize(touv({177,958}), touv({200,20}));

const UIbbox uibb_button = UIbbox::from_minsize(touv({177,938}), touv({200,20}));

const UIbbox uibb_buthover = UIbbox::from_minsize(touv({177,918}), touv({200,20}));

const UIbbox uibb_craft = UIbbox::from_minsize(touv({728,858}), touv({176,166}));

// const UIbbox uibb_ = UIbbox::from_minsize(touv({}), touv({}));



struct TestBox : UIelement {
    bool hovered = false;
    bool clicked = false;

    TestBox(UIbbox const& uv) {
        size = uv.size() * 1024.f * 4.f; /* times pixel dims times gui scale */
        uvbbox = uv;
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

    void draw() const override final {
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
    window.create("Minecraft UI Test", 1280, 720);

    UI ui;
    UIRenderer::init("ui"); // use the atlas texture named "ui"

    // Inventory UI - center pinned
    TestBox* inv = ui.alloc.push<TestBox>(uibb_inventory);
    inv->offset = glm::vec2(0.f, 0.f);
    ui.roots[UI::PIN_CENTERCENTER].addChild(inv);

    // Hotbar - pinned bottom center
    TestBox* hotbar = ui.alloc.push<TestBox>(uibb_hotbar);
    hotbar->offset = glm::vec2(0.f, hotbar->size.y / 2.f); // 10px above bottom
    ui.roots[UI::PIN_BOTCENTER].addChild(hotbar);

    // Selector over first slot
    // TestBox* selector = ui.alloc.push<TestBox>(uibb_hotbar_select);
    // selector->offset = glm::vec2(-91.f + 4.f, 10.f + 1.f); // align over first slot
    // ui.roots[UI::PIN_BOTCENTER].addChild(selector);

    // Hearts - left aligned above hotbar
    for (int i = 0; i < 10; ++i) {
        TestBox* heart = ui.alloc.push<TestBox>(uibb_heart);
        const float spacing = heart->size.x - 4.f;
        heart->offset = glm::vec2(-spacing * 5.f + i * spacing - (hotbar->size.x/4.f), (hotbar->size.y / 2.f) + (heart->size.y / 2.f));
        hotbar->addChild(heart);
    }

    // Food - right aligned above hotbar
    for (int i = 0; i < 10; ++i) {
        TestBox* food = ui.alloc.push<TestBox>(uibb_food);
        const float spacing = food->size.x - 4.f;
        food->offset = glm::vec2(spacing * 5.f - i * spacing + (hotbar->size.x/4.f), (hotbar->size.y / 2.f) + (food->size.y / 2.f));
        hotbar->addChild(food);
    }

    while (!window.should_close()) {
        if (window.keyboard[GLFW_KEY_ESCAPE].down) window.close();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        ui.tick(1.f / 60.f, window.mouse);
        UIRenderer::prepare(ui);
        UIRenderer::render();

        window.update();
    }

    gl.destroy();
    return 0;
}


