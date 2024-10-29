#pragma once

#include "Bindings.h"

class GameRegistry {
    public:
        ~GameRegistry();
        static void init();
        static uint32_t gregTextureIndex;
};