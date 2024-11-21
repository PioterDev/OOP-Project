#include "Game/Main/Game.hpp"
#include "Game/Main/GameObject.hpp"
#include "Game/PhysicalObject.hpp"
#include "Game/UIElement.hpp"

MainRegistry Game::registry;
GameRenderer Game::renderer;
InputHandler Game::inputHandler;

Game::~Game() {}

Status Game::init() {
    Status s = this->initSystems();
    if(s != Status::SUCCESS) return s;

    this->registry.init();

    return s;
}

void Game::run() {
    i64 start = 0, end = 0, delta = 0, overhead = 0, frameTime = 0;
    
    ///Section for testing ///
    UIElement obj(MainRegistry::someObjectID, MainRegistry::stoneTextureIndex);
    obj.setSizeOnScreen(200, 200).setPositionOnScreenCentered(640, 360).setBlendModulate().setModulation(255, 255, 0, 255).rotate(15.0).flipVertically();

    this->world.populateChunk({0, 0}, Blocks::cobblestone->getInstanceID());
    this->world.populateChunk({1, 1}, Blocks::cobblestone->getInstanceID());
    ///End of section for testing ///

    while(this->flags.running) {
        start = SDL_GetPerformanceCounter();
        frameTime = this->clockFrequency / this->renderer.fps;
        
        
        
        this->inputHandler.processInput(*this);
        this->renderer.renderInPlace(*this);
        

 
        end = SDL_GetPerformanceCounter();
        //Delta = Target frametime - Time elapsed - Overhead from previous frames
        delta = frameTime - (end - start) - overhead;
        delta = delta * (i64)1000 / (i64)this->clockFrequency;
        if(delta > 0) sleep(delta);

        end = SDL_GetPerformanceCounter();
        overhead = end - start - frameTime;
    }
}