#pragma once

#include "Bindings.h"

#include <vector>

#include "DSA/BitArray.hpp"
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
        static u32 gregTextureIndex;
        static u32 stoneTextureIndex;
        


        ////////////////////////
        /* Object IDs section */
        ////////////////////////
        static u32 someObjectID;
        

        
        ///////////////////////
        /* Block IDs section */
        ///////////////////////
        static u32 startingBlockID;
        static u32 endingBlockID;
        
};