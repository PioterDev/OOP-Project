#include "game.hpp"
#include "Game/UIElement.hpp"



UIElement::UIElement(
    const u32 objectID
) : RenderableObject(objectID) {
    Game::getRenderer().registerUIElement(this);
}

UIElement::UIElement(
    const u32 objectID, const char* name
) : RenderableObject(objectID, name) {
    Game::getRenderer().registerUIElement(this);
}

UIElement::UIElement(
    const u32 objectID,
    const u32 textureHandleIndex
) : RenderableObject(objectID, textureHandleIndex) {
    Game::getRenderer().registerUIElement(this);
}

UIElement::UIElement(
    const u32 objectID, const char* name, const u32 textureHandleIndex
) : RenderableObject(objectID, name, textureHandleIndex) {
    Game::getRenderer().registerUIElement(this);
}



void UIElement::render() {
    return; //for now no render logic
}

void UIElement::onHover() {}
void UIElement::onKeyPress(SDL_Keycode key) {}
void UIElement::onMouseClick(uint8_t button) {} 