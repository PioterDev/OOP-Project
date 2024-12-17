#pragma once

#include <cmath>

#include "Game/Render/RenderableObjectBase.hpp"
#include "program.hpp"

/**
 * @brief A renderable object is an object
 * with the capability of being rendered on the screen
 * with custom size, position, choosing a portion of the
 * texture to use, rotation, color modulation
 * and blending.
 */
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

        /**
         * @brief Color and alpha modulation applied
         * when rendering.
         * 
         * The modulation is applied using the following formula:
         * 
         * srcNew = srcOld * (mod / 255)
         * 
         * to the texture and removed from the texture after rendering.
         */
        Color colorModulation = {255, 255, 255, 255};

        /**
         * @brief Blend mode of the associated texture,
         * applied every render pass.
         */
        SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;
    public:
        RenderableObject(const u32 objectID);
        RenderableObject(const u32 objectID, const char* name);
        RenderableObject(const u32 objectID, const TextureHandle textureHandle);
        RenderableObject(const u32 objectID, const char* name, const TextureHandle textureHandle);

        ~RenderableObject() = default;
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
        SDL_Rect getTexturePortionCopy() const { return this->texturePortion; }

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
        SDL_Rect getTargetPortionCopy() const { return this->targetPortion; }

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
         * @param scale new horizontal scale. If `scale` is negative, it's not applied.
         * 
         * @return this, for chaining
         */
        RenderableObject& scaleX(const float scale);
        
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
         * @param scale new vertical scale. If `scale` is negative, it's not applied.
         * 
         * @return this, for chaining
         */
        RenderableObject& scaleY(const float scale);
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
         * @param scaleX new horizontal scale. If `scaleX` is negative, it's not applied.
         * @param scaleY new vertical scale. If `scaleY` is negative, it's not applied.
         * 
         * @return this, for chaining
         */
        RenderableObject& scale(const float scaleX, const float scaleY);
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
         * @param scale new scale. If `scale` is negative, it's not applied.
         * 
         * @return this, for chaining
         */
        RenderableObject& scale(const float scale);

        /**
         * @brief Rotates the object around its center.
         * 
         * @param degrees degrees to rotate
         */
        RenderableObject& rotate(const double degrees);

        /**
         * @brief Sets the object's upper left point's
         * position on the screen.
         * 
         * You should set the object's size on the screen before calling
         * this function, otherwise it will not be positioned correctly.
         * 
         * @param x horizontal coordinate
         * @param y vertical coordinate
         * 
         * @return this, for chaining
         */
        RenderableObject& setPositionOnScreen(const int x, const int y);
        /**
         * @brief Sets the object's center's position
         * on the screen. 
         * 
         * You should set the object's size on the screen before calling
         * this function, otherwise it will not be positioned correctly.
         * 
         * @param x horizontal coordinate
         * @param y vertical coordinate
         * 
         * @return this, for chaining
         */
        RenderableObject& setPositionOnScreenCentered(const int x, const int y);

        // /**
        //  * @brief Sets the object's upper left point's
        //  * position on the screen as a percentage of the
        //  * current window width and height.
        //  * 
        //  * @param pX 
        //  * @param pY 
        //  * @return RenderableObject& 
        //  */
        // RenderableObject& setPositionOnScreenRelative(float pX, float pY);

        // /**
        //  * @brief Sets the object's upper left point's
        //  * position on the screen as a percentage of the
        //  * current window width and height.
        //  * 
        //  * @param pX 
        //  * @param pY 
        //  * @return RenderableObject& 
        //  */
        // RenderableObject& setPositionOnScreenCenteredRelative(float pX, float pY);

        /**
         * @brief Sets the object's size on the screen, or the bottom
         * right point's position on the screen.
         * 
         * @param width screen width taken up by the object
         * @param height screen height taken up by the object
         * @return this, for chaining
         */
        RenderableObject& setSizeOnScreen(const u32 width, const u32 height);
        /**
         * @brief Sets the object's size on the screen, or the bottom
         * right point's position on the screen.
         * 
         * @param width screen width taken up by the object
         * @param height screen height taken up by the object
         * @return this, for chaining
         */
        RenderableObject& setSizeOnScreen(const Size size);

        /**
         * @brief Moves the object by a given offset on the screen.
         * 
         * @param dx offset in the X axis
         * @param dy offset in the Y axis
         * @return this, for chaining 
         */
        RenderableObject& moveOnScreen(const i32 dx, const i32 dy);

        /**
         * @brief Stretches the object by a given factor in the X axis.
         * 
         * The difference between `stretch` and `scale` is that `scale` uses
         * the size of a bound texture and applies scaling to it, while
         * `stretch` uses the current size on screen of the object.
         * 
         * @param factor by how much to multiply current object's width.
         * If `factor` is negative, it's not applied.
         * @return this, for chaining
         */
        RenderableObject& stretchX(const float factorX);
        /**
         * @brief Stretches the object by a given factor in the Y axis.
         *
         * The difference between `stretch` and `scale` is that `scale` uses
         * the size of a bound texture and applies scaling to it, while
         * `stretch` uses the current size on screen of the object.
         * 
         * @param factor by how much to multiply current object's height.
         * If `factor` is negative, it's not applied.
         * @return this, for chaining
         */
        RenderableObject& stretchY(const float factor);
        /**
         * @brief Stretches the object by a given factor in
         * the X and Y axis separately.
         * 
         * The difference between `stretch` and `scale` is that `scale` uses
         * the size of a bound texture and applies scaling to it, while
         * `stretch` uses the current size on screen of the object.
         * 
         * @param factorX by how much to multiply current object's width.
         * If `factor` is negative, it's not applied.
         * @param factorY by how much to multiply current object's height.
         * If `factor` is negative, it's not applied.
         * @return this, for chaining
         */
        RenderableObject& stretch(const float factorX, const float factorY);
        /**
         * @brief Stretches the object by a given factor in
         * both axes.
         * 
         * The difference between `stretch` and `scale` is that `scale` uses
         * the size of a bound texture and applies scaling to it, while
         * `stretch` uses the current size on screen of the object.
         * 
         * @param factor by how much to multiply current object's width and height.
         * If `factor` is negative, it's not applied.
         * @return this, for chaining
         */
        RenderableObject& stretch(const float factor);

        /**
         * @brief Stretches the object by a given number of pixels
         * in the X axis from its upper left point.
         * 
         * @param dx 
         * @return this, for chaining
         */
        RenderableObject& stretchX(const i32 dx);
        /**
         * @brief Stretches the object by a given number of pixels
         * in the Y axis from its upper left point.
         * 
         * @param dy 
         * @return this, for chaining
         */
        RenderableObject& stretchY(const i32 dy);
        /**
         * @brief Stretches the object by a given number of pixels
         * in the X and Y axis separately from its upper left point.
         * 
         * @param dx 
         * @param dy 
         * @return this, for chaining
         */
        RenderableObject& stretch(const i32 dx, const i32 dy);
        /**
         * @brief Stretches the object by a given number of pixels
         * in both axes from its upper left point.
         * 
         * @param d
         * @return this, for chaining
         */
        RenderableObject& stretch(const i32 d);

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
         * Unfortunately it's not possible to flip
         * the texture both horizontally and vertically
         * with this option. To do so, you need to
         * set the flip to none and rotate the texture 180 degrees.
         * 
         * @return this, for chaining
         */
        RenderableObject& flipHorizontally();
        /**
         * @brief Flips the object vertically.
         * 
         * If the object is flipped horizontally, does nothing.
         * Unfortunately it's not possible to flip
         * the texture both horizontally and vertically
         * with this option. To do so, you need to
         * set the flip to none and rotate the texture 180 degrees.
         * 
         * @return this, for chaining
         */
        RenderableObject& flipVertically();

        /**
         * @brief Sets the object's RGBA modifiers, multiplied into
         * the texture using the formula:
         * 
         * srcNew = srcOld * (mod / 255)
         * 
         * @param red red channel modifier
         * @param green green channel modifier
         * @param blue blue channel modifier
         * @param alpha alpha channel modifier
         * @return this, for chaining
         */
        RenderableObject& setModulation(const u8 red, const u8 green, const u8 blue, const u8 alpha);
        /**
         * @brief Sets the object's RGBA modifiers, multiplied into
         * the texture using the formula:
         * 
         * srcNew = srcOld * (mod / 255)
         * 
         * Usually used with setBlend... options.
         * 
         * @param rgba channel modifiers, glued into a single integer
         * @return this, for chaining
         */
        RenderableObject& setModulation(const u32 rgba);
        /**
         * @brief Sets the object's red channel modifier, multiplied into
         * the texture using the formula:
         * 
         * srcNew = srcOld * (mod / 255)
         * 
         * Usually used with setBlend... options.
         * 
         * @param mod modifier
         * @return this, for chaining
         */
        RenderableObject& setModulationRed(const u8 mod);
        /**
         * @brief Sets the object's green channel modifier, multiplied into
         * the texture using the formula:
         * 
         * srcNew = srcOld * (mod / 255)
         * 
         * Usually used with setBlend... options.
         * 
         * @param mod modifier
         * @return this, for chaining
         */
        RenderableObject& setModulationGreen(const u8 mod);
        /**
         * @brief Sets the object's blue channel modifier, multiplied into
         * the texture using the formula:
         * 
         * srcNew = srcOld * (mod / 255)
         * 
         * Usually used with setBlend... options.
         * 
         * @param mod modifier
         * @return this, for chaining
         */
        RenderableObject& setModulationBlue(const u8 mod);
        /**
         * @brief Sets the object's alpha channel modifier, multiplied into
         * the texture using the formula:
         * 
         * srcNew = srcOld * (mod / 255)
         * 
         * Usually used with setBlend... options.
         * 
         * @param mod modifier
         * @return this, for chaining
         */
        RenderableObject& setModulationAlpha(const u8 mod);

        /**
         * @brief Get the color modulation of the object as a struct.
         * 
         * @return Color struct, equivalent to internal data stored. 
         */
        Color getModulation() const { return this->colorModulation; }
        /**
         * @brief Get the color modulation of the object as an integer.
         * 
         * @return integer bitmask of colors and alpha channels
         */
        u32 getModulationInt() const { return *(u32*)(&this->colorModulation); }

        /**
         * @brief Sets the object's associated texture
         * blend mode to none.
         * 
         * @return this, for chaining
         */
        RenderableObject& setBlendNone();
        /**
         * @brief Sets the object's associated texture
         * blend mode to alpha:
         * 
         * dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA))
         * 
         * dstA = srcA + (dstA * (1-srcA))
         * 
         * @return this, for chaining 
         */
        RenderableObject& setBlendAlpha();
        /**
         * @brief Sets the object's associated texture
         * blend mode to additive:
         *
         * dstRGB = (srcRGB * srcA) + dstRGB
         * 
         * @return this, for chaining 
         */
        RenderableObject& setBlendAdditive();
        /**
         * @brief Sets the object's associated texture
         * blend mode to color modulate:
         * 
         * dstRGB = srcRGB * dstRGB
         * 
         * @return this, for chaining 
         */
        RenderableObject& setBlendModulate();
        /**
         * @brief Sets the object's associated texture
         * blend mode to alpha:
         * 
         * dstRGB = (srcRGB * dstRGB) + (dstRGB * (1-srcA))
         * 
         * @return this, for chaining 
         */
        RenderableObject& setBlendMultiplicative();
        /**
         * @brief Sets the object's associated texture
         * blend mode to the specified SDL_BlendMode.
         * 
         * @param blendMode blend mode from the SDL_BlendMode enum
         * 
         * @return this, for chaining 
         */
        RenderableObject& setBlendMode(const SDL_BlendMode blendMode);
        /**
         * @brief Get the object's associated blend mode.
         * 
         * @return SDL_BlendMode, usually used by the game's renderer
         */
        SDL_BlendMode getBlendMode() const { return this->blendMode; }
      
};