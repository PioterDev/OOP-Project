#include "Game/GameRenderer.hpp"
#include "game.hpp"

constexpr double sizeOfBlockTexture = 64.0;
//Don't use this, doesn't work that well.
/* void GameRenderer::render() {
    i64 frequency = (i64)SDL_GetPerformanceFrequency();
    i64 delta = 0, frameTime = 0;
    i64 overhead = 0; //if the thread sleeps for too long in one iteration, make it sleep shorter in another iteration
    i64 start, end;
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

void GameRenderer::renderInPlace(Game& game) {
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