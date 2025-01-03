#pragma once

#include "Bindings.h"

#include "Game/Render/RenderableObject.hpp"

/**
 * @brief A physical object is an object
 * with a position, size and a bounding box.
 */
class PhysicalObject : public RenderableObject {
    friend class ResourceManager;
    friend class GameRenderer;

    protected:
        Structs::Vector2d position;
        Structs::Point positionRounded;
        Structs::SizeF32 size;
        Structs::AABB aabb;
        
    public:
        /* PhysicalObject(const uint32_t objectID);
        PhysicalObject(const uint32_t objectID, const uint32_t textureHandleIndex);
        PhysicalObject(
            const uint32_t objectID, const uint32_t textureHandleIndex,
            const Vector2f initialPosition, const SizeF32 size
        ); */


        const Structs::AABB& getAABB() const { return this->aabb; }
        Structs::AABB getAABBCopy() const { return this->aabb; }
        PhysicalObject& setAABB() {
            this->aabb.x1 = this->position.x - this->size.width;
            this->aabb.x2 = this->position.x + this->size.width;
            this->aabb.y1 = this->position.y - this->size.height;
            this->aabb.y2 = this->position.y + this->size.height;
            return *this;
        }
        Structs::Vector2d getPosition() const { return this->position; }
        PhysicalObject& setPosition(Structs::Vector2d pos) {
            this->position = pos;
            this->positionRounded.x = (i32)round(pos.x);
            this->positionRounded.y = (i32)round(pos.y);
            return *this;
        }
        Structs::Point getPositionRounded() const { return this->positionRounded; }
        Structs::SizeF32 getSize() const { return this->size; }
        PhysicalObject& setSize(Structs::SizeF32 size) {
            this->size = size;
            return *this;
        }
};