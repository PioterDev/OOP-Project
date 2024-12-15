#pragma once

//TODO: finish implementing a renderable static text box
#include "Game/Render/UIElement.hpp"

/**
 * @brief A UITextElement is an element
 * of a UI which contains logic for
 * rendering non-modifiable text boxes.
 */
class UITextElement : UIElement {
    public:
        UITextElement(const u32 objectID);
        UITextElement(const u32 objectID, const char* name);
        UITextElement(const u32 objectID, const TextureHandle textureHandle);
        UITextElement(const u32 objectID, const char* name, const TextureHandle textureHandle);

        ~UITextElement() = default;


};