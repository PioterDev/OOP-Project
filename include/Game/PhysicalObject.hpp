#pragma once

#include "Bindings.h"

#include <cmath>

#include "Game/Render/RenderableObject.hpp"

/**
 * @brief A physical object is an object
 * with a position, size and a bounding box.
 */
class PhysicalObject : public RenderableObject {
    friend class ResourceManager;
    friend class GameRenderer;

    protected:
        Vector2f position;
        Point positionRounded;
        SizeF32 size;
        AABB aabb;
        
    public:
        /* PhysicalObject(const uint32_t objectID);
        PhysicalObject(const uint32_t objectID, const uint32_t textureHandleIndex);
        PhysicalObject(
            const uint32_t objectID, const uint32_t textureHandleIndex,
            const Vector2f initialPosition, const SizeF32 size
        ); */


        AABB getAABB() const { return this->aabb; }
        PhysicalObject& setAABB() {
            this->aabb.x1 = this->position.x - this->size.width;
            this->aabb.x2 = this->position.x + this->size.width;
            this->aabb.y1 = this->position.y - this->size.height;
            this->aabb.y2 = this->position.y + this->size.height;
            return *this;
        }
        Vector2f getPosition() const { return this->position; }
        PhysicalObject& setPosition(Vector2f pos) {
            this->position = pos;
            this->positionRounded.x = (int32_t)roundf(pos.x);
            this->positionRounded.y = (int32_t)roundf(pos.y);
            return *this;
        }
        Point getPositionRounded() const { return this->positionRounded; }
        SizeF32 getSize() const { return this->size; }
        PhysicalObject& setSize(SizeF32 size) {
            this->size = size;
            return *this;
        }
};