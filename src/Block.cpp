#include "Game/Block/Block.hpp"
#include "Game/Block/Blocks.hpp"

using namespace Structs;

const AABB Block::standardBlockAABB = {0.0, 0.0, 1.0, 1.0};

Block::Block(const u32 objectID) : RenderableObjectBase(objectID) {
    this->setInstanceID(Blocks::getCurrentBlockID());
    Blocks::addBlock(this);
}

Block::Block(
    const u32 objectID,
    const char* name
) : RenderableObjectBase(objectID, name) {
    this->setInstanceID(Blocks::getCurrentBlockID());
    Blocks::addBlock(this);
}

Block::Block(
    const u32 objectID,
    const TextureHandle textureHandle
) : RenderableObjectBase(objectID, textureHandle) {
    this->setInstanceID(Blocks::getCurrentBlockID());
    Blocks::addBlock(this);
}

Block::Block(
    const u32 objectID,
    const char* name,
    const TextureHandle textureHandle
) : RenderableObjectBase(objectID, name, textureHandle) {
    this->setInstanceID(Blocks::getCurrentBlockID());
    Blocks::addBlock(this);
}

Block& Block::setDensity(float density) {
    this->density = density;
    return *this;
}