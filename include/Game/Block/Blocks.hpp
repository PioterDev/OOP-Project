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
        static u32 numberOfBlocksRegistered;
    public:
        static void addBlock(Block* block);

        static u32 getCurrentBlockID();

        static Block* getBlockWithID(u32 id);

        static u32 init(u32 blockID);
        
        static Block* cobblestone;
};