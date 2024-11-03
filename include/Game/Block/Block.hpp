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
        Block(const uint32_t objectID);
        Block(const uint32_t objectID, const char* name);
        Block(const uint32_t objectID, const uint32_t textureHandleIndex);
        Block(const uint32_t objectID, const char* name, const uint32_t textureHandleIndex);

        Block& setDensity(float density);

        void render() override {}
};