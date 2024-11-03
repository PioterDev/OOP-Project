#include "game.hpp"
#include "Game/UIElement.hpp"

UIElement::UIElement(
    const uint32_t objectID,
    const uint32_t textureHandleIndex
) : RenderableObject(objectID, textureHandleIndex) {
    Game::getRenderer().registerUIElement(this);
}

UIElement::UIElement(
    const uint32_t objectID
) : RenderableObject(objectID) {
    Game::getRenderer().registerUIElement(this);
}

void UIElement::render() {
    return;
}

void UIElement::onHover() {}
void UIElement::onKeyPress(SDL_Keycode key) {}
void UIElement::onMouseClick(uint8_t button) {} 