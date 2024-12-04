#include "Game/Main/Game.hpp"
#include "Game/UIElement.hpp"



UIElement::UIElement(
    const u32 objectID
) : RenderableObject(objectID) {}

UIElement::UIElement(
    const u32 objectID,
    const char* name
) : RenderableObject(objectID, name) {}

UIElement::UIElement(
    const u32 objectID,
    const TextureHandle textureHandle
) : RenderableObject(objectID, textureHandle) {}

UIElement::UIElement(
    const u32 objectID,
    const char* name,
    const TextureHandle textureHandle
) : RenderableObject(objectID, name, textureHandle) {}

void UIElement::render() {
    this->moveOnScreen(
        0,
        (i32)(5.0 * sin(0.05 * Game::getRenderer().getNumberOfFramesRendered()))
    );
    this->rotate(1.0);
    return; //for now no render logic
}

void UIElement::onHover() {}
void UIElement::onKeyPress(SDL_Keycode key) {}
void UIElement::onMouseClick(uint8_t button) {}


/**
 * These exist since UIElements have to be accessible
 * by the game's renderer, so it's either pointer
 * nightmare and unnecessary allocations or
 * yielding ownership to the renderer and accepting
 * some inconvenience.
 */

UIElement& UIElement::createUIElement(const u32 objectID) {
    return Game::getRenderer().registerUIElement(UIElement(objectID));
}
UIElement& UIElement::createUIElement(const u32 objectID, const char* name) {
    return Game::getRenderer().registerUIElement(UIElement(objectID, name));
}
UIElement& UIElement::createUIElement(const u32 objectID, const TextureHandle textureHandle) {
    return Game::getRenderer().registerUIElement(UIElement(objectID, textureHandle));
}
UIElement& UIElement::createUIElement(const u32 objectID, const char* name, const TextureHandle textureHandle) {
    return Game::getRenderer().registerUIElement(UIElement(objectID, name, textureHandle));
}