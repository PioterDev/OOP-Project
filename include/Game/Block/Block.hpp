#pragma once

#include "Bindings.h"

#include "deus.hpp"
#include "Game/Main/GameObject.hpp"
#include "Game/Render/RenderableObjectBase.hpp"

using namespace Structs;

/**
 * @brief A block is the most basic
 * representation of a part of the world it
 * resides in.
 */
class Block : public RenderableObjectBase {
    protected:
        /**
         * @brief Material density of the block
         */
        float density;
    public:
        static const AABB standardBlockAABB;
        
        explicit Block(const u32 objectID);
        explicit Block(const u32 objectID, const char* name);
        explicit Block(const u32 objectID, const TextureHandle textureHandle);
        explicit Block(const u32 objectID, const char* name, const TextureHandle textureHandle);

        Block& setDensity(float density);

        virtual void render() override {} //blocks do not use render() by default
};