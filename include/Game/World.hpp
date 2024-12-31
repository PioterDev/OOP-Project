#pragma once

#include "Bindings.h"

#include <cstdlib>
#include <unordered_map>

#include "deus.hpp"
#include "Game/Block/Block.hpp"
#include "Game/Block/Blocks.hpp"
#include "Game/Main/GameObject.hpp"
#include "program.hpp"

typedef struct {
    u32 blockIDs[16][16];
} Chunk;

class PointHash {
    public: size_t operator()(const Structs::ChunkPos& p) const { return p.x * p.x + p.y * p.y; }
};

class World : public GameObject {
    protected:
        std::unordered_map<Structs::ChunkPos, Chunk*, PointHash> chunks;
    public:
        explicit World(const u32 objectID) : GameObject(objectID), chunks(256) {}
        explicit World(const u32 objectID, const char* name) : GameObject(objectID, name), chunks(256) {}
        
        ~World() {
            for(auto& [point, chunk] : this->chunks) {
                free(chunk);
            }
        }

        const Chunk* getChunk(const i32 x, const i32 y) { return this->chunks[{x, y}]; }
        const Chunk* getChunk(const Structs::ChunkPos which) { return this->chunks[which]; }
        
        Enums::Status populateChunk(const Structs::ChunkPos which, const u32 blockID);

        /**
         * @brief Get the Block at given position.
         * 
         * @param x 
         * @param y 
         * @return const Block* or NULL if there is no block
         */
        const Block* getBlockAt(i32 x, i32 y);
        /**
         * @brief Get the Block at given position.
         * 
         * @param pos block position
         * @return const Block* or NULL if there's no block
         */
        const Block* getBlockAt(Structs::BlockPos pos) { return this->getBlockAt(pos.x, pos.y); }

        void printChunk(i32 x, i32 y);
        void printChunk(Structs::ChunkPos pos) { this->printChunk(pos.x, pos.y); }

};