#pragma once

#include "Bindings.h"

#include <chrono>
#include <thread>

#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "deus.hpp"
#include "logging.hpp"
#include "resources.hpp"

using namespace Enums;
using namespace Structs;

using std::array;
// #undef sleep
// #define sleep(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

struct alignas(8) ProgramFlags {
    u8 SDL_Initialized : 1 = false;
    u8 SDL_Image_Initialized : 1 = false;
    u8 SDL_Mixer_Initialized : 1 = false;
    u8 SDL_TTF_Initalized : 1 = false;
    u8 canPlaySound : 1 = false;
    u8 running : 1 = false;
};

typedef struct {
    string name = "Window";
    Size size = {1280, 720};
} WindowParameters;

struct alignas(4) AudioParameters {
    u8 audioChannelAmount = 16; //16 audio channels by default
    u8 masterVolume = 64; //Half of maximum by default
    u8 musicVolume = 64; //Also half of maximum by default
    u8 sfxVolume = 64; //Again, half of maximum by default
};

class Program {
    public:
        ~Program();
        
        /**
         * @brief Initializes the program's required systems.
         * 
         * @return status of startup - 0 is success, everything else is mapped to Enums::Status
         */
        NoDiscard Status initSystems();

        static u64 getClockFrequency() { return Program::clockFrequency; }

        static SDL_Renderer* getRenderingContext() { return renderingContext; }
        static Logger& getLogger() { return logger; }
        // static InputHandler& getInputHandler() { return inputHandler; }
        static ResourceManager& getResourceManager() { return resourceManager; }

        static Size getScreenSize() { return windowParameters.size; }

        /**
         * @brief Get the window associated with the program.
         * 
         * @return SDL_Window*, this is a handle to internal resource,
         * do not modify
         */
        SDL_Window* getWindow() { return this->window; }

        u32 getNumberOfKeys() const { return this->numberOfKeyboardKeys; }

        Point getMousePosition() const { return this->mousePosition; }
        void updateMouse() {
            this->mouseButtons = SDL_GetMouseState(
                &this->mousePosition.x, &this->mousePosition.y
            );
        }

        static const uint8_t* keyboardState;
    protected:
        struct ProgramFlags flags;

        u32 numberOfKeyboardKeys;

        /* static thread_local string latestError;
        static thread_local Status latestErrorCode; */

        static u64 clockFrequency;
        u64 timeSinceStart;

        //Video-related members
        SDL_Window* window;
        static WindowParameters windowParameters;
        static SDL_Renderer* renderingContext;
        Point mousePosition;
        u32 mouseButtons;

        //Audio-related members
        struct AudioParameters audioParameters;
        
        static Logger logger;
        // static InputHandler inputHandler;
        static ResourceManager resourceManager;
};