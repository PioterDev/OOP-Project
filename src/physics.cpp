/* #include "game.hpp"
#include "Game/PhysicalObject.hpp"

PhysicalObject::PhysicalObject(
    const uint32_t objectID,
    const uint32_t textureHandleIndex,
    const Point2D_f32 initialPosition,
    const SizeF32 size
) : RenderableObject(objectID, textureHandleIndex) {
    Game::getRenderer().registerPhysicalObject(this);
    this->position = initialPosition;
    this->size = size;
    this->setAABB();
} */