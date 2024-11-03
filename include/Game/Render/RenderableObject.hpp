#pragma once

#include <cmath>

#include "Game/Render/RenderableObjectBase.hpp"
#include "program.hpp"

//TODO: struct for temporary render data
typedef struct {

} PerFrameRenderData;

class RenderableObject : public RenderableObjectBase {
    friend class ResourceManager;
    friend class GameRenderer;

    protected:
        /**
         * @brief Rectangle used to select
         * which portion of the texture
         * should be used in rendering.
         */
        SDL_Rect texturePortion = {0, 0, 0, 0};
        /**
         * @brief Rectangle used to select
         * which portion of the rendering target will be filled
         * with the texture.
         */
        SDL_Rect targetPortion = {0, 0, 0, 0};
        /**
         * @brief Angle at which the texture should be rotated
         * when rendering.
         */
        double angle = 0.0;
        /**
         * @brief What flip will be applied when rendering.
         * 
         * Unfortunately it's not possible to flip
         * the texture both horizontally and vertically
         * with this option. To do so, you need to
         * set the flip to none and rotate the texture 180 degrees.
         */
        SDL_RendererFlip flip = SDL_FLIP_NONE;

    public:
        RenderableObject(const uint32_t objectID);
        RenderableObject(const uint32_t objectID, const char* name);
        RenderableObject(const uint32_t objectID, const uint32_t textureHandleIndex);
        RenderableObject(const uint32_t objectID, const char* name, const uint32_t textureHandleIndex);

        /**
         * @brief Sets the texture's portion to use.
         * 
         * A texture portion is part of the texture to use in rendering,
         * as opposed to a target portion, which positions the texture on
         * the screen.
         * 
         * @param r rectangle portion of the texture
         * 
         * @return this, for chaining
         */
        RenderableObject& setTexturePortion(const SDL_Rect& r);

        /**
         * @brief Sets the texture's portion to use
         * to the whole texture.
         * 
         * @return this, for chaining
         */
        RenderableObject& setTexturePortionOriginal();

        const SDL_Rect& getTexturePortion() const { return this->texturePortion; }

        /**
         * @brief Sets the target portion.
         * 
         * A target portion is a part of the screen to fill
         * with the texture, as opposed to a texture portion,
         * which selects relevant part of the source texture to copy.
         * 
         * @param r rectangle portion of the screen
         * 
         * @return this, for chaining
         */
        RenderableObject& setTargetPortion(const SDL_Rect& r);

        const SDL_Rect& getTargetPortion() const { return this->targetPortion; }

        /**
         * @brief Scales the texture in the X axis (horizontally).
        
         * You can safely ignore the following information 
         * if you won't have to change the texture throughout
         * the lifetime of the object.
         * 
         * Scaling is applied immediately and is not retained throughout
         * different texture handles (i.e. the texture's size
         * will not change if for whatever reason
         * the texture handle is changed and its original size changes).
         * @param scale new horizontal scale
         * 
         * @return this, for chaining
         */
        RenderableObject& scaleX(float scale);
        
        /**
         * @brief Scales the texture in the Y axis (vertically).
         * 
         * You can safely ignore the following information 
         * if you won't have to change the texture throughout
         * the lifetime of the object.
         * 
         * Scaling is applied immediately and is not retained throughout
         * different texture handles (i.e. the texture's size
         * will not change if for whatever reason
         * the texture handle is changed and its original size changes).
         * 
         * @param scale new vertical scale
         * 
         * @return this, for chaining
         */
        RenderableObject& scaleY(float scale);
        /**
         * @brief Scales the texture.
         * 
         * You can safely ignore the following information 
         * if you won't have to change the texture throughout
         * the lifetime of the object.
         * 
         * Scaling is applied immediately and is not retained throughout
         * different texture handles (i.e. the texture's size
         * will not change if for whatever reason
         * the texture handle is changed and its original size changes).
         * 
         * @param scale new scale
         * 
         * @return this, for chaining
         */
        RenderableObject& scale(float scale);
        /**
         * @brief Scales the texture separately in the
         * X and Y axes.
         * 
         * You can safely ignore the following information 
         * if you won't have to change the texture throughout
         * the lifetime of the object.
         * 
         * Scaling is applied immediately and is not retained throughout
         * different texture handles (i.e. the texture's size
         * will not change if for whatever reason
         * the texture handle is changed and its original size changes).
         * 
         * @param scaleX new horizontal scale
         * @param scaleY new vertical scale
         * 
         * @return this, for chaining
         */
        RenderableObject& scale(float scaleX, float scaleY);

        /**
         * @brief Rotates the object around its center.
         * 
         * @param degrees degrees to rotate
         */
        void rotate(double degrees);
        /**
         * @brief Sets the object's upper left point's
         * position on the screen.
         * 
         * @param x horizontal coordinate
         * @param y vertical coordinate
         * 
         * @return this, for chaining
         */
        RenderableObject& setPositionOnScreen(int x, int y);
        /**
         * @brief Sets the object's center's position
         * on the screen. 
         * 
         * @param x horizontal coordinate
         * @param y vertical coordinate
         * 
         * @return this, for chaining
         */
        RenderableObject& setPositionOnScreenCentered(int x, int y);

        /**
         * @brief Sets the object's bottom right point's
         * position on the screen.
         * 
         * @param width screen width taken up by the object
         * @param height screen height taken up by the object
         * @return this, for chaining
         */
        RenderableObject& setSizeOnScreen(uint32_t width, uint32_t height);

        /**
         * @brief Sets the object's flip to none.
         * 
         * @return this, for chaining
         */
        RenderableObject& unflip();
        /**
         * @brief Flips the object horizontally.
         * 
         * If the object is flipped vertically, does nothing.
         * See this->textureData.flip for an explanation.
         * 
         * @return this, for chaining
         */
        RenderableObject& flipHorizontally();
        /**
         * @brief Flips the object vertically.
         * 
         * If the object is flipped horizontally, does nothing.
         * See this->textureData.flip for an explanation.
         * 
         * @return this, for chaining
         */
        RenderableObject& flipVertically();
};