#pragma once

#include "Bindings.h"

#include <vector>

#include "BitArray.hpp"
#include "deus.hpp"

using std::vector;

using namespace Enums;
using namespace Unions;
using namespace Structs;

typedef struct {
    char* name;
    double gravity;
} WorldProperties;

class MainRegistry {
    private:
        /**
         * @brief Determines whether an object with some ID has a registered
         * override for some property.
         */
        BitArray overrides;
    public:
        MainRegistry();
        ~MainRegistry();
        static void init();
        


        /////////////////////////
        /* Texture IDs section */
        /////////////////////////
        static uint32_t gregTextureIndex;
        static uint32_t stoneTextureIndex;
        


        ////////////////////////
        /* Object IDs section */
        ////////////////////////
        static uint32_t someObjectID;
        

        
        ///////////////////////
        /* Block IDs section */
        ///////////////////////
        static uint32_t startingBlockID;
        static uint32_t endingBlockID;
        
};