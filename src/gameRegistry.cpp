#include "Game/Main/MainRegistry.hpp"
#include "program.hpp"
#include "Game/Block/Blocks.hpp"

const char* defaultObjectName = "";

/////////////////////////
/* Texture IDs section */
/////////////////////////
uint32_t MainRegistry::gregTextureIndex;
uint32_t MainRegistry::stoneTextureIndex;



////////////////////////
/* Object IDs section */
////////////////////////
uint32_t MainRegistry::someObjectID;



///////////////////////
/* Block IDs section */
///////////////////////

//ID of the first registered block type
uint32_t MainRegistry::startingBlockID;
//ID of the last registered block type + 1
uint32_t MainRegistry::endingBlockID;



MainRegistry::MainRegistry() : overrides(1024) {}

MainRegistry::~MainRegistry() {}

void MainRegistry::init() {

    //This will be moved into a different source file eventually...
    /////////////////////////
    /* Texture IDs section */
    /////////////////////////
    MainRegistry::gregTextureIndex = Program::getResourceManager().registerTexture("./assets/bzns.png");
    MainRegistry::stoneTextureIndex = Program::getResourceManager().registerTexture("./assets/cobblestone.png");



    ////////////////////////
    /* Object IDs section */
    ////////////////////////


    uint32_t lowestID = 16; //first 16 are reserved
    ///////////////////////
    /* Block IDs section */
    ///////////////////////
    MainRegistry::startingBlockID = 16;
    lowestID = MainRegistry::endingBlockID = Blocks::init(lowestID);
}