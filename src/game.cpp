#include "game.hpp"
#include "Game/GameObject.hpp"
#include "Game/PhysicalObject.hpp"
#include "Game/UIElement.hpp"

MainRegistry Game::registry;
GameRenderer Game::renderer;
InputHandler Game::inputHandler;

Game::~Game() {}

Status Game::init() {
    Status s = Status::SUCCESS;
    s = this->initSystems();
    if(s != Status::SUCCESS) return s;

    this->registry.init();

    return s;
}

void Game::run() {
    // std::thread renderThread = this->renderer.startRender();
    i64 start = 0, end = 0, delta = 0, overhead = 0, frameTime = 0;
    
    UIElement obj(MainRegistry::someObjectID, MainRegistry::gregTextureIndex);
    obj.setSizeOnScreen(100, 100).setPositionOnScreenCentered(640, 360);

    this->world.populateChunk({0, 0}, Blocks::cobblestone->getInstanceID());
    this->world.populateChunk({1, 1}, Blocks::cobblestone->getInstanceID());
    while(this->flags.running) {
        start = SDL_GetPerformanceCounter();
        frameTime = this->clockFrequency / this->renderer.fps;
        
        
        
        this->inputHandler.processInput(*this);
        this->renderer.renderInPlace(*this);
        

 
        end = SDL_GetPerformanceCounter();
        //delta = target frametime - time elapsed - overhead from previous frames
        delta = frameTime - (end - start) - overhead;
        delta = delta * (i64)1000 / (i64)this->clockFrequency;
        if(delta > 0) sleep(delta);

        end = SDL_GetPerformanceCounter();
        overhead = end - start - frameTime;
    }

    // renderThread.join();
}