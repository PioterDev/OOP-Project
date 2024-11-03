#include "Game/GameRenderer.hpp"
#include "game.hpp"

constexpr double sizeOfBlockTexture = 64.0;
//Don't use this, doesn't work that well.
/* void GameRenderer::render() {
    int64_t frequency = (int64_t)SDL_GetPerformanceFrequency();
    int64_t delta = 0, frameTime = 0;
    int64_t overhead = 0; //if the thread sleeps for too long in one iteration, make it sleep shorter in another iteration
    int64_t start, end;
    double scalingFactor = 1.0;

    while(true) {
        start = SDL_GetPerformanceCounter();

        //if(this->status == ThreadStatus::STOP) break;
        //else if(this->status == ThreadStatus::PAUSE) goto loop_end;

        frameTime = frequency / this->fps;
        scalingFactor = this->scalingFactor;

        SDL_RenderClear(Program::getRenderingContext());
        SDL_SetRenderDrawColor(Program::getRenderingContext(), 20, 20, 20, SDL_ALPHA_OPAQUE);

        this->physicalObjectsToRender.forEach([](PhysicalObject* object) {
            if(!object->isVisible()) return;
            //TODO: add temporary render data passed to object->render()
            object->render();
            SDL_RenderCopyEx(
                Program::getRenderingContext(),
                object->getTexture(),
                &object->texturePortion,
                &object->targetPortion,
                object->angle,
                nullptr,
                object->flip
            );
        });

        this->uiElements.forEach([](UIElement* element) {
            if(!element->isVisible()) return;
            element->render();
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
        
        loop_end:
        end = SDL_GetPerformanceCounter();

        //delta = target frametime - time elapsed
        delta = frameTime - (end - start) - overhead;
        delta = delta * 1000 / frequency; //to get miliseconds
        if(delta > 0) sleep(delta);

        end = SDL_GetPerformanceCounter();
        overhead = end - start - frameTime;
    }
} */

void GameRenderer::renderInPlace(Game* game) {
    this->start = SDL_GetPerformanceCounter();
    this->frameTime = Program::getClockFrequency() / this->fps;
    Size screenSize = Program::getScreenSize();
    SDL_Rect r;

    SDL_RenderClear(Program::getRenderingContext());
    SDL_SetRenderDrawColor(Program::getRenderingContext(), 80, 80, 80, SDL_ALPHA_OPAQUE);

    /* this->physicalObjectsToRender.forEach([](PhysicalObject* object) {
        //TODO: add temporary render data passed to object->render()
        if(!object->isVisible()) return;
        object->render();
        
        SDL_RenderCopyEx(
            Program::getRenderingContext(),
            object->getTexture(),
            &object->texturePortion,
            &object->targetPortion,
            object->angle,
            nullptr,
            object->flip
        );
    }); */
    double pixelsPerBlock = sizeOfBlockTexture * this->scalingFactor;
    int32_t pixelsPerBlockInt = (int32_t)pixelsPerBlock;
    /* Point topLeftVisibleChunkPosition = {
        floorf((float)this->cameraPosition.x / pixelsPerChunk),
        -floorf((float)this->cameraPosition.y / pixelsPerChunk)
    };
    Point numberOfChunksToRender = {0, 0};
    for(int32_t i = this->cameraPosition.x; i <= this->cameraPosition.x + (int32_t)screenSize.width; i += pixelsPerChunkInt) {
        numberOfChunksToRender.x++;
    }
    for(int32_t i = this->cameraPosition.y; i <= this->cameraPosition.y + (int32_t)screenSize.height; i += pixelsPerChunkInt) {
        numberOfChunksToRender.y++;
    }

    for(int32_t i = topLeftVisibleChunkPosition.x; i < topLeftVisibleChunkPosition.x + numberOfChunksToRender.x; i++) {
        for(int32_t j = topLeftVisibleChunkPosition.y; j < topLeftVisibleChunkPosition.y + numberOfChunksToRender.y; j--) {
            const Chunk* currentChunk = game->getWorld().getChunk(i, j);
            if(currentChunk == nullptr) continue;
            int32_t k = (int32_t)floorf((float)this->cameraPosition.x / pixelsPerBlock);
            if(k < 0)
        }
    } */
    BlockPos topLeftVisibleBlock = {
        (int32_t)floor((double)this->cameraPosition.x / pixelsPerBlock),
        (int32_t)floor((double)this->cameraPosition.y / pixelsPerBlock)
    };

    r.w = r.h = pixelsPerBlockInt;
    r.y = topLeftVisibleBlock.y * pixelsPerBlockInt - this->cameraPosition.y;
    for(
        int32_t i = topLeftVisibleBlock.y;
        i <= topLeftVisibleBlock.y + (int32_t)ceil((double)screenSize.height/pixelsPerBlock);
        i++, r.y += pixelsPerBlockInt
    ) {
        r.x = topLeftVisibleBlock.x * pixelsPerBlockInt - this->cameraPosition.x;
        for(
            int32_t j = topLeftVisibleBlock.x;
            j <= topLeftVisibleBlock.x + (int32_t)ceil((double)screenSize.width/pixelsPerBlock);
            j++, r.x += pixelsPerBlockInt
        ) {
            const Block* block = game->getWorld().getBlockAt(j, -i);
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
    
    // loop_end:
    this->end = this->lastFrameAt = SDL_GetPerformanceCounter();
    this->numberOfFramesRendered++;

    //delta = target frametime - time elapsed - overhead from previous frames
    this->delta = this->frameTime - (this->end - this->start) - this->overhead;
    this->delta = this->delta * (int64_t)1000 / (int64_t)Program::getClockFrequency(); //to get miliseconds
    if(this->delta > 0) sleep(this->delta);

    
    this->end = SDL_GetPerformanceCounter();
    this->overhead = this->end - this->start - this->frameTime;
}

void GameRenderer::moveCamera(int32_t offX, int32_t offY) {
    this->cameraPosition.x += offX;
    this->cameraPosition.y += offY;
}