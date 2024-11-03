#include "Game/Block/Block.hpp"
#include "Game/Block/Blocks.hpp"

const AABB Block::blockAABB = {0.0, 0.0, 1.0, 1.0};

Block::Block(const uint32_t objectID) : RenderableObjectBase(objectID) {
    this->setInstanceID(Blocks::getCurrentBlockID());
    Blocks::addBlock(this);
}

Block::Block(
    const uint32_t objectID,
    const char* name
) : RenderableObjectBase(objectID, name) {
    this->setInstanceID(Blocks::getCurrentBlockID());
    Blocks::addBlock(this);
}

Block::Block(
    const uint32_t objectID,
    const uint32_t textureHandleIndex
) : RenderableObjectBase(objectID, textureHandleIndex) {
    this->setInstanceID(Blocks::getCurrentBlockID());
    Blocks::addBlock(this);
}

Block::Block(
    const uint32_t objectID,
    const char* name,
    const uint32_t textureHandleIndex
) : RenderableObjectBase(objectID, name, textureHandleIndex) {
    this->setInstanceID(Blocks::getCurrentBlockID());
    Blocks::addBlock(this);
}

Block& Block::setDensity(float density) {
    this->density = density;
    return *this;
}