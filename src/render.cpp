#include "Game/GameRenderer.hpp"
#include "Game/Main/Game.hpp"

constexpr double sizeOfBlockTexture = 64.0;

void GameRenderer::renderInPlace(Game& game) {
    Size screenSize = Program::getScreenSize();
    SDL_Rect r;

    SDL_RenderClear(Program::getRenderingContext());
    SDL_SetRenderDrawColor(Program::getRenderingContext(), 80, 80, 80, SDL_ALPHA_OPAQUE);

    double pixelsPerBlock = sizeOfBlockTexture * this->scalingFactor;
    i32 pixelsPerBlockInt = (i32)pixelsPerBlock;

    BlockPos topLeftVisibleBlock = {
        (i32)floor((double)this->cameraPosition.x / pixelsPerBlock),
        (i32)floor((double)this->cameraPosition.y / pixelsPerBlock)
    };

    r.w = r.h = pixelsPerBlockInt;
    r.y = topLeftVisibleBlock.y * pixelsPerBlockInt - this->cameraPosition.y;
    for(
        i32 i = topLeftVisibleBlock.y;
        i <= topLeftVisibleBlock.y + (i32)ceil((double)screenSize.height/pixelsPerBlock);
        i++, r.y += pixelsPerBlockInt
    ) {
        r.x = topLeftVisibleBlock.x * pixelsPerBlockInt - this->cameraPosition.x;
        for(
            i32 j = topLeftVisibleBlock.x;
            j <= topLeftVisibleBlock.x + (i32)ceil((double)screenSize.width/pixelsPerBlock);
            j++, r.x += pixelsPerBlockInt
        ) {
            const Block* block = game.getWorld().getBlockAt(j, -i);
            if(block == nullptr) continue;
            if(block->getTexture() == nullptr) continue;

            SDL_RenderCopy(
                Program::getRenderingContext(),
                block->getTexture(),
                nullptr, &r
            );
        }
    }

    this->uiElements.forEach([](UIElement* element) {
        if(!element->isVisible()) return;
        element->render();
        if(element->getTexture() == nullptr) return;

        SDL_RenderCopyEx(
            Program::getRenderingContext(),
            element->getTexture(),
            &element->texturePortion,
            &element->targetPortion,
            element->angle,
            nullptr,
            element->flip
        );
    });

    SDL_RenderPresent(Program::getRenderingContext());
    
    this->lastFrameAt = SDL_GetPerformanceCounter();
    this->numberOfFramesRendered++;
}

void GameRenderer::moveCamera(i32 offX, i32 offY) {
    this->cameraPosition.x += offX;
    this->cameraPosition.y += offY;
}