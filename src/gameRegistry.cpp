#include "Game/GameRegistry.hpp"
#include "program.hpp"

uint32_t GameRegistry::gregTextureIndex;

GameRegistry::~GameRegistry() {}

void GameRegistry::init() {
    gregTextureIndex = Program::getResourceManager().registerTexture("./assets/greg.png");
}