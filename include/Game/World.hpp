#pragma once

#include "Bindings.h"

#include <cstdlib>
#include <unordered_map>

#include "deus.hpp"
#include "Game/Block/Block.hpp"
#include "Game/Block/Blocks.hpp"
#include "Game/Main/GameObject.hpp"
#include "program.hpp"

using std::unordered_map;

using namespace Enums;

typedef struct {
    u32 blockIDs[16][16];
} Chunk;

class PointHash {
    public: size_t operator()(const ChunkPos& p) const { return p.x * p.x + p.y * p.y; }
};

class World : public GameObject {
    protected:
        unordered_map<ChunkPos, Chunk*, PointHash> chunks;
    public:
        World(const u32 objectID) : GameObject(objectID), chunks(256) {}
        World(const u32 objectID, const char* name) : GameObject(objectID, name), chunks(256) {}
        
        ~World() {
            for(auto& [point, chunk] : this->chunks) {
                free(chunk);
            }
        }

        const Chunk* getChunk(const i32 x, const i32 y) { return this->chunks[{x, y}]; }
        const Chunk* getChunk(const ChunkPos which) { return this->chunks[which]; }
        
        Status populateChunk(const ChunkPos which, const u32 blockID);

        const Block* getBlockAt(i32 x, i32 y);
        const Block* getBlockAt(BlockPos pos) { return this->getBlockAt(pos.x, pos.y); }

        void printChunk(i32 x, i32 y);
        void printChunk(ChunkPos pos) { this->printChunk(pos.x, pos.y); }

};