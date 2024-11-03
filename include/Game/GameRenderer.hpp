#pragma once

#include "Bindings.h"

#include "Game/UIElement.hpp"
#include "Game/PhysicalObject.hpp"
#include "ListArray.hpp"
#include "program.hpp"

// void startRender(RenderThreadParameters* params);
class Game;

class GameRenderer {
    friend class Game;
    friend class InputHandler;

    private:
        ListArray<PhysicalObject*> physicalObjectsToRender;
        ListArray<UIElement*> uiElements;
        
        uint32_t fps = 144;
        int64_t start, end, delta, overhead, frameTime;
        double scalingFactor = 1.0;

        //Registers the timestamp when rendering
        //the latest frame has ended
        uint64_t lastFrameAt;
        //Counts the number of frames since rendering started
        uint64_t numberOfFramesRendered = 0;
        
        Point cameraPosition = {0, 0};

        void moveCamera(int32_t offX, int32_t offY);
    public:
        GameRenderer() : physicalObjectsToRender(1024), uiElements(1024) {}

        void registerUIElement(UIElement* element) { uiElements.append(element); }

        void registerPhysicalObject(PhysicalObject* obj) { physicalObjectsToRender.append(obj); }

        // void render();
        void renderInPlace(Game* game);

        // std::thread startRender() {
        //     return std::thread([this] { this->render(); });
        // }

        uint64_t getTimeSinceLastFrame() const { return SDL_GetPerformanceCounter() - this->lastFrameAt; }

        Point getCameraPosition() const { return this->cameraPosition; }
};