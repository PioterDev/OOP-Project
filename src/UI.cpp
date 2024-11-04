#include "game.hpp"
#include "Game/UIElement.hpp"

UIElement::UIElement(
    const u32 objectID,
    const u32 textureHandleIndex
) : RenderableObject(objectID, textureHandleIndex) {
    Game::getRenderer().registerUIElement(this);
}

UIElement::UIElement(
    const u32 objectID
) : RenderableObject(objectID) {
    Game::getRenderer().registerUIElement(this);
}

void UIElement::render() {
    return;
}

void UIElement::onHover() {}
void UIElement::onKeyPress(SDL_Keycode key) {}
void UIElement::onMouseClick(uint8_t button) {} 