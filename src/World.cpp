#include "Game/World.hpp"

Status World::populateChunk(const ChunkPos which, const u32 blockID) {
    Chunk* chunk = this->chunks[which];
    if(chunk != nullptr) return Status::ALREADY_EXISTS;

    chunk = (Chunk*)malloc(sizeof(Chunk));
    if(chunk == nullptr) return Status::ALLOC_FAILURE;

    //for now the entire chunk will be made with just 1 block
    //this will change when a proper procedural world generation
    //will be implemented
    memset(chunk, blockID, sizeof(Chunk));
    
    this->chunks[which] = chunk;
    return Status::SUCCESS;
}

const Block* World::getBlockAt(i32 x, i32 y) {
    const Chunk* chunk = this->getChunk(x >= 0 ? x/16 : x/16 - 1, y >= 0 ? y/16 : y/16 - 1);
    if(chunk == nullptr) return nullptr;

    return Blocks::getBlockWithID(
        chunk->blockIDs[y >= 0 ? y%16 : y%16 + 16][x >= 0 ? x%16 : x%16 + 16]
    );
}

void World::printChunk(i32 x, i32 y) {
    Logger& logger = Program::getLogger();
    const Chunk* chunk = this->chunks[{x, y}];
    if(chunk == nullptr) {
        logger.print("No chunk\n");
        return;
    }
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 16; j++) {
            logger.print(chunk->blockIDs[i][j], " ");
        }
        logger.print("\n");
    }
}