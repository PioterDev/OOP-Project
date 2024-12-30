#pragma once

#include "Bindings.h"

#include "Game/Render/UIElement.hpp"
#include "Game/Physics/PhysicalObject.hpp"
#include "DSA/ListArray.hpp"
#include "program.hpp"

// void startRender(RenderThreadParameters* params);
class Game;

class GameRenderer {
    friend class Game;
    friend class InputHandler;

    private:
        ListArray<PhysicalObject*> physicalObjectsToRender;
        ListArray<UIElement> uiElements;
        
        u32 fps = 144;
        double scalingFactor = 1.0;

        //Registers the timestamp when rendering
        //the latest frame has ended
        u64 lastFrameAt = 0;
        //Counts the number of frames since rendering started
        u64 numberOfFramesRendered = 0;
        
        Point cameraPosition = {0, 0};

        void moveCamera(i32 offX, i32 offY);
    public:
        GameRenderer() : physicalObjectsToRender(4096), uiElements(4096) {}

        UIElement& registerUIElement(UIElement element) { return uiElements.append(std::move(element)); }

        // void registerPhysicalObject(PhysicalObject* obj) { physicalObjectsToRender.append(std::move(obj)); }

        // void render();
        void renderInPlace(Game& game);

        // std::thread startRender() {
        //     return std::thread([this] { this->render(); });
        // }

        u64 getTimeSinceLastFrame() const { return SDL_GetPerformanceCounter() - this->lastFrameAt; }
        u64 getNumberOfFramesRendered() const { return this->numberOfFramesRendered; }

        Point getCameraPosition() const { return this->cameraPosition; }
};