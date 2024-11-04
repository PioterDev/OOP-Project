#pragma once

#include "Bindings.h"

#include "deus.hpp"
#include "Game/GameObject.hpp"
#include "Game/Render/RenderableObjectBase.hpp"

using namespace Structs;

class Block : public RenderableObjectBase {
    protected:
        static const AABB blockAABB;
        /**
         * @brief Material density of the block
         */
        float density;
    public:
        Block(const u32 objectID);
        Block(const u32 objectID, const char* name);
        Block(const u32 objectID, const u32 textureHandleIndex);
        Block(const u32 objectID, const char* name, const u32 textureHandleIndex);

        Block& setDensity(float density);

        void render() override {}
};