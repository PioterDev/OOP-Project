#pragma once

#include "Bindings.h"

#include "deus.hpp"
#include "Game/RenderableObject.hpp"

/**
 * @brief A physical object is an object
 * with physical properties like mass, volume, density,
 * temperature, chemical structure, etc.
 * 
 * 
 * Inherits from RenderableObject, meaning any can, but won't
 * necessarily be rendered (like air).
 */
class PhysicalObject : public RenderableObject {
    protected:
        Point2D_f32 aabb;
};