#include "game.hpp"
#include "Game/GameObject.hpp"
#include "Game/RenderableObject.hpp"

GameRegistry Game::registry;
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

RenderableObject::RenderableObject(uint32_t textureHandleIndex) {
    this->setVisible();
    Game::getRenderer().registerRenderableObject(this);
    this->textureData.bindTexture(
        Program::getResourceManager().getTextureHandle(
            textureHandleIndex
        )
    );
    this->textureData.setTexturePortionOriginal();
    
    this->textureData.targetPortion.w = 256;
    this->textureData.targetPortion.h = 256;
    this->textureData.targetPortion.x = 1280/2 - 256/2;
    this->textureData.targetPortion.y = 720/2 - 256/2;
}

RenderableObject::RenderableObject(uint32_t textureHandleIndex, const SDL_Rect& r) {
    this->setVisible();
    Game::getRenderer().registerRenderableObject(this);
    this->textureData.bindTexture(
        Program::getResourceManager().getTextureHandle(
            textureHandleIndex
        )
    );
    this->textureData.setTexturePortionOriginal();
    this->textureData.targetPortion = r;
}

void Game::run() {
    // std::thread renderThread = this->renderer.startRender();
    RenderableObject obj(GameRegistry::gregTextureIndex);
    RenderableObject* objs[16];
    SDL_Rect r = {0, 0, 64, 64};
    for(int i = 0; i < 16; i++) {
        objs[i] = new RenderableObject(GameRegistry::gregTextureIndex, r);
        r.x += 64;
    }
    while(this->flags.running) {
        this->inputHandler.processInput(this);
        this->renderer.renderInPlace();
    }

    for(int i = 0; i < 16; i++) {
        delete objs[i];
    }
    // renderThread.join();
}