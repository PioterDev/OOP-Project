#pragma once

#include "Bindings.h"

#include <cstdlib>
#include <unordered_map>

#include "deus.hpp"
#include "Game/Block/Block.hpp"
#include "Game/Block/Blocks.hpp"
#include "Game/GameObject.hpp"
#include "program.hpp"

using std::unordered_map;

using namespace Enums;

typedef struct {
    uint32_t blockIDs[16][16];
} Chunk;

class PointHash {
    public: size_t operator()(const Point& p) const { return p.x * p.x + p.y + p.y; }
};

class World : public GameObject {
    protected:
        unordered_map<Point, Chunk*, PointHash> chunks;
    public:
        World(const uint32_t objectID) : GameObject(objectID), chunks(256) {}
        World(const uint32_t objectID, const char* name) : GameObject(objectID, name), chunks(256) {}
        
        ~World() {
            for(auto& [point, chunk] : this->chunks) {
                free(chunk);
            }
        }

        const Chunk* getChunk(const Point which) { return this->chunks[which]; }
        const Chunk* getChunk(const int32_t x, const int32_t y) { return this->chunks[{x, y}]; }
        
        Status populateChunk(const Point which, const uint32_t blockID);

        const Block* getBlockAt(int32_t x, int32_t y);
        const Block* getBlockAt(BlockPos pos) { return this->getBlockAt(pos.x, pos.y); }

        void printChunk(int32_t x, int32_t y);

};