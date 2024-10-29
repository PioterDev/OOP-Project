#pragma once

#include <cmath>

#include "Game/GameObject.hpp"
#include "program.hpp"
#include "resources.hpp"

/**
 * @brief A generic game object that is
 * able to be rendered.
 * 
 * Reserves flag 0:
 * 
 * 0 - is the object visible
 */
class RenderableObject : public GameObject {
    friend class ResourceManager;
    friend class GameRenderer;

    private:
        Texture textureData;
    public:
        RenderableObject(uint32_t textureHandleIndex);

        RenderableObject(uint32_t textureHandleIndex, const SDL_Rect& r);

        virtual void render() {
            if(this->isVisible()) {
                this->rotate(1.0);
            }
        };

        bool isVisible() { return this->isFlagSet(0); }
        void setVisible() { this->setFlag(0); }
        void setInvisible() { this->clearFlag(0); }
        void flipVisible() { this->flipFlag(0); }

        void rotate(double degrees) {
            if(this->textureData.angle > 360.0) {
                this->textureData.angle -= 360.0;
            }
            this->textureData.angle += degrees;
        }
};