#include "game.hpp"
#include "Game/GameObject.hpp"
#include "Game/PhysicalObject.hpp"
#include "Game/UIElement.hpp"

MainRegistry Game::registry;
GameRenderer Game::renderer;
InputHandler Game::inputHandler;

GameObject::~GameObject() {}

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
    UIElement obj(MainRegistry::someObjectID, MainRegistry::gregTextureIndex);
    obj.setSizeOnScreen(640, 360).setPositionOnScreenCentered(640, 360);

    this->world.populateChunk({0, 0}, Blocks::cobblestone->getInstanceID());
    while(this->flags.running) {
        this->inputHandler.processInput(this);
        this->renderer.renderInPlace(this);
    }

    // renderThread.join();
}