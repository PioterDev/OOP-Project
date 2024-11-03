#pragma once

#include "Bindings.h"

#include "deus.hpp"
#include "Game/Render/RenderableObject.hpp"

/**
 * @brief A UI element is a renderable object
 * that is directly interactable via input from
 * a keyboard, mouse, controller, etc.
 */
class UIElement : public RenderableObject {
    public:
        UIElement(const uint32_t objectID);
        UIElement(const uint32_t objectID, const uint32_t textureHandleIndex);

        virtual void onHover();
        virtual void onKeyPress(SDL_Keycode key);
        virtual void onMouseClick(uint8_t button);

        void render();
};