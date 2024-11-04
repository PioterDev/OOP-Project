#pragma once

#include "Game/GameObject.hpp"
#include "resources.hpp"
#include "program.hpp"

/**
 * @brief A base class for renderable objects.
 * 
 * Reserves flag 1:
 * 
 * 1 - is the object visible
 */
class RenderableObjectBase : public GameObject {
    friend class ResourceManager;
    friend class GameRenderer;

    protected:
        TextureHandle textureHandle;
    public:
        RenderableObjectBase(const u32 objectID);
        RenderableObjectBase(const u32 objectID, const char* name);
        RenderableObjectBase(const u32 objectID, const u32 textureHandleIndex);
        RenderableObjectBase(const u32 objectID, const char* name, const u32 textureHandleIndex);
        
        /**
         * @brief "Renders" the object.
         * 
         * It does not, actually. This function only optionally sets
         * up render data for the game's renderer, meaning that if it's
         * totally empty, the object will still be rendered. To avoid this,
         * make it invisible, so that the renderer will skip it.
         */
        virtual void render() = 0;
        
        /**
         * @brief Checks if the object is visible.
         * If false, it won't be rendered.
         * 
         * The object is visible by default, even if
         * no texture is bound to it.
         */
        bool isVisible() const { return this->isFlagSet(1); }
        void setVisible() { this->setFlag(1); }
        void setInvisible() { this->clearFlag(1); }
        void flipVisible() { this->flipFlag(1); }

        /**
         * @brief Get the raw pointer to the texture bound to
         * this object.
         * 
         * @return Pointer to texture or nullptr if no texture is bound
         */
        SDL_Texture* getTexture() const { return this->textureHandle.texture; }

        /**
         * @brief Binds a texture to the object.
         * 
         * @param textureHandleIndex obtained from the texture registry
         * @return this, for chaining
         */
        RenderableObjectBase& bindTexture(u32 textureHandleIndex);
};