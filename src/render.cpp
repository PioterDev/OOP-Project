#include "Game/GameRenderer.hpp"
#include "program.hpp"

void GameRenderer::render() {
    int64_t frequency = (int64_t)SDL_GetPerformanceFrequency();
    int64_t delta = 0, frameTime = 0;
    int64_t overhead = 0; //if the thread sleeps for too long in one iteration, make it sleep shorter in another iteration
    int64_t start, end;
    double scalingFactor = 1.0;

    while(true) {
        start = SDL_GetPerformanceCounter();

        if(this->status == ThreadStatus::STOP) break;
        else if(this->status == ThreadStatus::PAUSE) goto loop_end;

        frameTime = frequency / this->fps;
        scalingFactor = this->scalingFactor;

        SDL_RenderClear(Program::getRenderingContext());
        SDL_SetRenderDrawColor(Program::getRenderingContext(), 200, 20, 80, SDL_ALPHA_OPAQUE);
        this->objectsToRender.forEach([](RenderableObject* object) {
            object->render();
            SDL_RenderCopyEx(
                Program::getRenderingContext(),
                object->textureData.getTexture(),
                &object->textureData.rect,
                &object->textureData.targetPortion,
                object->textureData.angle,
                nullptr,
                object->textureData.flip
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
}

void GameRenderer::renderInPlace() {
    int64_t frequency = (int64_t)SDL_GetPerformanceFrequency();

    this->start = SDL_GetPerformanceCounter();
    this->frameTime = frequency / this->fps;
    // scalingFactor = this->scalingFactor;

    SDL_RenderClear(Program::getRenderingContext());
    SDL_SetRenderDrawColor(Program::getRenderingContext(), 200, 20, 80, SDL_ALPHA_OPAQUE);
    this->objectsToRender.forEach([](RenderableObject* object) {
        object->render();
        SDL_RenderCopyEx(
            Program::getRenderingContext(),
            object->textureData.getTexture(),
            &object->textureData.rect,
            &object->textureData.targetPortion,
            object->textureData.angle,
            nullptr,
            object->textureData.flip
        ); 
    });
    SDL_RenderPresent(Program::getRenderingContext());
    
    // loop_end:
    this->end = SDL_GetPerformanceCounter();

    //delta = target frametime - time elapsed
    this->delta = this->frameTime - (this->end - this->start) - this->overhead;
    this->delta = this->delta * 1000 / frequency; //to get miliseconds
    if(this->delta > 0) sleep(this->delta);
    
    this->end = SDL_GetPerformanceCounter();
    this->overhead = this->end - this->start - this->frameTime;
}