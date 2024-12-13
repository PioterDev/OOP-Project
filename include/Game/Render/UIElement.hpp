#pragma once

#include "Bindings.h"

#include "deus.hpp"
#include "Game/Render/RenderableObject.hpp"

/**
 * @brief A UI element is a renderable object
 * that is directly interactable via input from
 * a keyboard, mouse, controller, etc.
 * 
 * Try not to create them with a constructor as
 * it will not be hooked to the game's renderer.
 * Use `createUIElement` or a similar method instead.
 */
class UIElement : public RenderableObject {
    friend class GameRenderer;
    public:
        UIElement(const u32 objectID);
        UIElement(const u32 objectID, const char* name);
        UIElement(const u32 objectID, const TextureHandle textureHandle);
        UIElement(const u32 objectID, const char* name, const TextureHandle textureHandle);

        ~UIElement() = default;

        virtual void onHover();
        virtual void onKeyPress(SDL_Keycode key);
        virtual void onMouseClick(uint8_t button);

        virtual void render() override;

        static UIElement& createUIElement(const u32 objectID);
        static UIElement& createUIElement(const u32 objectID, const char* name);
        static UIElement& createUIElement(const u32 objectID, const TextureHandle textureHandle);
        static UIElement& createUIElement(const u32 objectID, const char* name, const TextureHandle textureHandle);
};