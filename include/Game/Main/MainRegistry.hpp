#pragma once

#include "Bindings.h"

#include "deus.hpp"
#include "DSA/BitArray.hpp"

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
        

        /////////////////////////
        /*   Font IDs section  */
        /////////////////////////
        static u32 consolasFontIndex;

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