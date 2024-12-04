#include "Game/GameRenderer.hpp"
#include "Game/Main/Game.hpp"

constexpr double sizeOfBlockTexture = 64.0;

void GameRenderer::renderInPlace(Game& game) {
    Size windowSize = game.getWindowSize();
    SDL_Rect r;
    Color backgroundColor = game.getBackgroundColor();

    SDL_RenderClear(game.getRenderingContext());
    SDL_SetRenderDrawColor(
        game.getRenderingContext(),
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
                game.getRenderingContext(),
                block->getTexture(),
                nullptr, &r
            );
        }
    }
    /// End of block rendering ///

    //"&game = game" bruh, lambdas are weird
    this->uiElements.forEach([&game = game](UIElement& element) {
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
            game.getRenderingContext(),
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

    SDL_RenderPresent(game.getRenderingContext());
    
    this->lastFrameAt = SDL_GetPerformanceCounter();
    this->numberOfFramesRendered++;
}

void GameRenderer::moveCamera(i32 offX, i32 offY) {
    this->cameraPosition.x += offX;
    this->cameraPosition.y += offY;
}