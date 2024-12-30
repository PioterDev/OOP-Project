#include "Game/Main/MainRegistry.hpp"
#include "program.hpp"
#include "Game/Block/Blocks.hpp"

/////////////////////////
/* Texture IDs section */
/////////////////////////
u32 MainRegistry::gregTextureIndex;
u32 MainRegistry::stoneTextureIndex;

/////////////////////////
/*   Font IDs section  */
/////////////////////////
u32 MainRegistry::consolasFontIndex;


////////////////////////
/* Object IDs section */
////////////////////////
u32 MainRegistry::someObjectID;



///////////////////////
/* Block IDs section */
///////////////////////

//ID of the first registered block type
u32 MainRegistry::startingBlockID;
//ID of the last registered block type + 1
u32 MainRegistry::endingBlockID;



MainRegistry::MainRegistry() : overrides(1024) {}

MainRegistry::~MainRegistry() {}

void MainRegistry::init() {

    //This will be moved into a different source file eventually...
    /////////////////////////
    /* Texture IDs section */
    /////////////////////////
    MainRegistry::gregTextureIndex = Program::getResourceManager().registerTexture("./assets/abruz.png", TextureFlags_LoadImmediately);
    MainRegistry::stoneTextureIndex = Program::getResourceManager().registerTexture("./assets/cobblestone.png", TextureFlags_LoadImmediately);

    /////////////////////////
    /*   Font IDs section  */
    /////////////////////////
    MainRegistry::consolasFontIndex = Program::getResourceManager().loadFont("./assets/segoeuil.ttf", 128);

    ////////////////////////
    /* Object IDs section */
    ////////////////////////


    u32 lowestID = 16; //first 16 are reserved
    ///////////////////////
    /* Block IDs section */
    ///////////////////////
    MainRegistry::startingBlockID = 16;
    lowestID = MainRegistry::endingBlockID = Blocks::init(lowestID);
}