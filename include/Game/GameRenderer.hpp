#pragma once

#include "Bindings.h"

#include "Game/RenderableObject.hpp"
#include "ListArray.hpp"
#include "program.hpp"

// void startRender(RenderThreadParameters* params);

class GameRenderer {
    friend class Game;
    friend class InputHandler;

    private:
        ListArray<RenderableObject*> objectsToRender;
        uint32_t fps = 144;
        ThreadStatus status = ThreadStatus::CONTINUE;
        int64_t start, end, delta, overhead, frameTime;
        double scalingFactor = 1.0;
    public:
        GameRenderer() : objectsToRender(1024) {}

        void registerRenderableObject(RenderableObject* obj) {
            objectsToRender.append(obj);
        }

        void render();
        void renderInPlace();

        std::thread startRender() {
            return std::thread([this] { this->render(); });
        }
};