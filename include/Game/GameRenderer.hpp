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
        
        u32 fps = 144;
        i64 start, end, delta, overhead, frameTime;
        double scalingFactor = 1.0;

        //Registers the timestamp when rendering
        //the latest frame has ended
        u64 lastFrameAt;
        //Counts the number of frames since rendering started
        u64 numberOfFramesRendered = 0;
        
        Point cameraPosition = {0, 0};

        void moveCamera(i32 offX, i32 offY);
    public:
        GameRenderer() : physicalObjectsToRender(1024), uiElements(1024) {}

        void registerUIElement(UIElement* element) { uiElements.append(element); }

        void registerPhysicalObject(PhysicalObject* obj) { physicalObjectsToRender.append(obj); }

        // void render();
        void renderInPlace(Game* game);

        // std::thread startRender() {
        //     return std::thread([this] { this->render(); });
        // }

        u64 getTimeSinceLastFrame() const { return SDL_GetPerformanceCounter() - this->lastFrameAt; }

        Point getCameraPosition() const { return this->cameraPosition; }
};