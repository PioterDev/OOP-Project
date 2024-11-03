#include "Game/Block/Blocks.hpp"
#include "Game/Main/MainRegistry.hpp"

vector<Block*> Blocks::blocks;
uint32_t Blocks::numberOfBlocksRegistered = 0;

Block* Blocks::cobblestone = nullptr;

/**
 * @brief This is uber ugly, but there's
 * literally NO OTHER WAY TO STATICALLY
 * INITIALIZE STUFF AFTER SOME FUNCTION
 * GETS EXECUTED.
 * 
 * So yeah, it's pointer hell.
 * 
 * This function will likely grow to several
 * thousand lines...
 * 
 * This function should only be executed after
 * registering textures.
 * 
 * To maintain UUID cohesion, this function receives a
 * lowest available ID reserving IDs for blocks in
 * the central registry and returns the lowest available
 * ID for other stuff.
 * 
 * Each block is its own instance of the `Block` class,
 * meaning blocks will be identified by their instance ID,
 * not type ID. The instance ID will start from 0 and increment
 * at every block registration. The final number is the number of
 * blocks registered.
 * 
 * There is another problem, however (sort of):
 * because different block types will have different
 * type IDs, but every block pointer is placed into the same
 * vector, block types that are not registered with initial `blockID`
 * will have gaps in instance IDs. This isn't realistically a problem as
 * the ID space can hold 2^32 instances, but it's worth mentioning.
 * 
 * Another thing worth mentioning is that changing the order at which
 * blocks are registered will break compatibility with already existing
 * worlds.
 */
uint32_t Blocks::init(uint32_t blockID) {
    Blocks::blocks.reserve(1024);
    static Block cobblestone(blockID, "Cobblestone", MainRegistry::stoneTextureIndex);
    Blocks::cobblestone = &cobblestone;

    return blockID + 1;
}

void Blocks::addBlock(Block* block) {
    Blocks::blocks.push_back(block);
    Blocks::numberOfBlocksRegistered++;
}

uint32_t Blocks::getCurrentBlockID() {
    return numberOfBlocksRegistered;
}

Block* Blocks::getBlockWithID(uint32_t id) {
    if(Blocks::blocks.size() >= id) return Blocks::blocks[0];
    return Blocks::blocks[id];
}