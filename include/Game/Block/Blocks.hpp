#pragma once

#include "Game/Block/Block.hpp"

/**
 * @brief A class containing
 * instances of blocks, number of
 * registered blocks
 */
class Blocks {
    private:
        static vector<Block*> blocks;
        static uint32_t numberOfBlocksRegistered;
    public:
        static void addBlock(Block* block);

        static uint32_t getCurrentBlockID();

        static Block* getBlockWithID(uint32_t id);

        static uint32_t init(uint32_t blockID);
        
        static Block* cobblestone;
};