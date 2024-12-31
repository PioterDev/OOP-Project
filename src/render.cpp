#include "Game/GameRenderer.hpp"
#include "Game/Main/Game.hpp"
#include "Math.hpp"

using namespace Structs;

static constexpr double sizeOfBlockTexture = 64.0;

/**
 * @brief Draws an arc on the current rendering target.
 * Used for drawing circular and elliptical arcs, as well as
 * circles and ellipses.
 * To draw a circular arc, set `rX` equal to `rY`.
 * 
 * @param renderer rendering context
 * @param center point around which the arc will be drawn
 * @param rX horizontal radius of the arc
 * @param rY vertical radius of the arc
 * @param start start of the arc in degrees, automatically clamped to [0, 360] degrees
 * @param end end of the arc in degrees, automatically clamped to [0, 360] degrees;
 * draws counterclockwise if `end < start`
 * @param centerColor color of the central point
 * @param outerColor color of outer points; if `outerColor` is different than
 * `centerColor` a gradient is applied between them
 * @param srcTex (optional) texture to copy pixel data from, if
 * `centerColor` and `outerColor` are not white, a mask from these colors is applied
 * @param srcTexCenter (optional) center point of the part of `srcTex`
 * to copy
 */
void DrawArc(
    SDL_Renderer* renderer, PointF center, float rX, float rY,
    double start, double end, Color centerColor, Color outerColor,
    SDL_Texture* srcTex, PointF srcTexCenter
) {
    float inv_w = 0.0f, inv_h = 0.0f;
    if(srcTex) {
        int texWidth, texHeight;
        if(SDL_QueryTexture(srcTex, nullptr, nullptr, &texWidth, &texHeight)) return;
        inv_w = 1.0f / (float)texWidth; inv_h = 1.0f / (float)texHeight;
        if(srcTexCenter.x + rX > (float)texWidth || srcTexCenter.x - rX < 0.0f) return; //out-of-bounds
        if(srcTexCenter.y + rY > (float)texHeight || srcTexCenter.y - rY < 0.0f) return; //out-of-bounds
    }
    constexpr double step = 1.0;

    while(start < 0.0) { start += 360.0; }
    while(start > 360.0) { start -= 360.0; }

    while(end < 0.0) { end += 360.0; }
    while(end > 360.0) { end -= 360.0; }

    double angle = end - start;
    if(angle < 0.0) angle += 360.0;
    
    u32 steps = (u32)(floor(abs(angle) * 10.0) * 0.1 * (1.0 / step));
    SDL_Vertex vertices[(size_t)(360.0 / step) + 2];
    int indices[3 * (size_t)(360.0 / step)];

    vertices[0].position = {center.x, center.y};
    vertices[0].color = *(SDL_Color*)&centerColor;
    if(srcTex) vertices[0].tex_coord = {srcTexCenter.x * inv_w, srcTexCenter.y * inv_h};
    {
        u32 i;
        float t;
        if(start > end) {
            for(i = 1, t = (float)(end + 360.0); i < steps + 2; i++, t -= (float)step) {
                Vector2f off = {
                    rX * Math::SinTable<(size_t)(90.0 / step), float>::at(t),
                    -rY * Math::CosTable<(size_t)(90.0 / step), float>::at(t)
                };
                vertices[i].position = {
                    center.x + off.x,
                    center.y + off.y
                };
                vertices[i].color = *(SDL_Color*)&outerColor;
                if(srcTex) {
                    vertices[i].tex_coord = {
                        (srcTexCenter.x + off.x) * inv_w,
                        (srcTexCenter.y + off.y) * inv_h,
                    };
                }
            }
        }
        else {
            for(i = 1, t = (float)start; i < steps + 2; i++, t += (float)step) {
                Vector2f off = {
                    rX * Math::SinTable<(size_t)(90.0 / step), float>::at(t),
                    -rY * Math::CosTable<(size_t)(90.0 / step), float>::at(t)
                };
                vertices[i].position = {
                    center.x + off.x,
                    center.y + off.y
                };
                vertices[i].color = *(SDL_Color*)&outerColor;
                if(srcTex) {
                    vertices[i].tex_coord = {
                        (srcTexCenter.x + off.x) * inv_w,
                        (srcTexCenter.y + off.y) * inv_h,
                    };
                }
            }
        }
    }
    for(u32 i = 0, j = 1; i < 3 * steps; i += 3, j++) {
        indices[i] = 0;
        indices[i + 1] = j;
        indices[i + 2] = j + 1;
    }

    SDL_RenderGeometry(renderer, srcTex, vertices, steps + 2, indices, 3 * steps);
}

ForceInline void DrawCircle(
    SDL_Renderer* renderer, PointF center, float r,
    Color centerColor, Color outerColor,
    SDL_Texture* srcTex, PointF srcTexCenter
) {
    DrawArc(renderer, center, r, r, 0.0, 360.0, centerColor, outerColor, srcTex, srcTexCenter);
}

ForceInline void DrawEllipse(
    SDL_Renderer* renderer, PointF center, float rX, float rY,
    Color centerColor, Color outerColor,
    SDL_Texture* srcTex, PointF srcTexCenter
) {
    DrawArc(renderer, center, rX, rY, 0.0, 360.0, centerColor, outerColor, srcTex, srcTexCenter);
}

void GameRenderer::renderInPlace(Game& game) {
    Size windowSize = game.getWindowSize();
    SDL_Rect r;
    Color backgroundColor = game.getBackgroundColor();
    SDL_Renderer* renderer = game.getRenderingContext();

    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(
        renderer,
        backgroundColor.red,
        backgroundColor.green,
        backgroundColor.green,
        backgroundColor.alpha
    );


    /// Block rendering ///
    double pixelsPerBlock = sizeOfBlockTexture * this->scalingFactor;
    i32 pixelsPerBlockInt = (i32)pixelsPerBlock;

    BlockPos topLeftVisibleBlock = {
        (i32)floor((double)this->cameraPosition.x / pixelsPerBlock),
        (i32)floor((double)this->cameraPosition.y / pixelsPerBlock)
    };

    r.w = r.h = pixelsPerBlockInt;
    r.y = topLeftVisibleBlock.y * pixelsPerBlockInt - this->cameraPosition.y;
    //The renderer iterates over blocks vertically and horizontally
    //starting from the upper-left visible one
    for(
        i32 i = topLeftVisibleBlock.y;
        i <= topLeftVisibleBlock.y + (i32)ceil((double)windowSize.height/pixelsPerBlock);
        i++, r.y += pixelsPerBlockInt
    ) {
        r.x = topLeftVisibleBlock.x * pixelsPerBlockInt - this->cameraPosition.x;
        for(
            i32 j = topLeftVisibleBlock.x;
            j <= topLeftVisibleBlock.x + (i32)ceil((double)windowSize.width/pixelsPerBlock);
            j++, r.x += pixelsPerBlockInt
        ) {
            const Block* block = game.getWorld().getBlockAt(j, -i);
            if(block == nullptr) continue;
            if(block->getTexture() == nullptr) continue;

            SDL_RenderCopy(
                renderer,
                block->getTexture(),
                nullptr, &r
            );
        }
    }
    /// End of block rendering ///

    this->uiElements.forEach([renderer = renderer](UIElement& element) {
        if(!element.isVisible()) return;
        element.render();

        SDL_Texture* texture = element.getTexture();
        if(!texture) return;
        
        Color colorMod = element.getModulation();
        SDL_BlendMode blendMode = element.getBlendMode();
        
        //A hacky way to check if all fields in colorMod are 255
        if(*((i32*)&colorMod) != (i32)-1) {
            SDL_SetTextureColorMod(texture, colorMod.red, colorMod.green, colorMod.blue);
            SDL_SetTextureAlphaMod(texture, colorMod.alpha);
        }
        
        if(blendMode != SDL_BLENDMODE_NONE) {
            SDL_SetTextureBlendMode(texture, blendMode);
        }

        SDL_RenderCopyEx(
            renderer,
            element.getTexture(),
            &element.texturePortion,
            &element.targetPortion,
            element.angle,
            nullptr, //might later add a center point to renderable objects I guess
            element.flip
        );
        
        //restore original color modulation
        if(*((i32*)&colorMod) != (i32)-1) {
            SDL_SetTextureColorMod(texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(texture, 255);
        }

        if(blendMode != SDL_BLENDMODE_NONE) {
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
        }
    });

    ///    Section for render testing     ///
    static double start = 0.0, end = 270.0;
    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320, 320);

    Size s = Program::getResourceManager().getTextureOriginalSize(MainRegistry::gregTextureIndex);
    PointF p = {(float)(s.width / 2), (float)(s.height / 2)};
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, tex);
    DrawArc(
        renderer, {160.0f, 160.0f}, 160.0f, 160.0f, start, end, Colors::YELLOW, Colors::MAGENTA,
        Program::getResourceManager().getTexture(MainRegistry::gregTextureIndex), p
    );

    // DrawCircle(renderer, {250.0f, 250.0f}, 125.0f, {255,255,255,0}, {255,255,255,0}, nullptr, {});
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_Rect rr = {0, 0, 320, 320};
    SDL_RenderCopy(renderer, tex, nullptr, &rr);
    SDL_DestroyTexture(tex);
    start += 1.0;
    end += 1.0;
    /// End of section for render testing ///

    //TODO: formalize text rendering into a separate entity
    //This can get complicated though as adding anything
    //to the string to show requires recreating the entire texture
    //but alas, let's hope that's a rare circumstance

    SDL_RenderPresent(renderer);
    
    this->lastFrameAt = SDL_GetPerformanceCounter();
    this->numberOfFramesRendered++;
}

void GameRenderer::moveCamera(i32 offX, i32 offY) {
    this->cameraPosition.x += offX;
    this->cameraPosition.y += offY;
}