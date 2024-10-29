#pragma once

#include "Bindings.h"

#include <chrono>
#include <thread>
#include <SDL.h>
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

struct WindowParameters {
    string name = "Window";
    Size size = {1280, 720};
};

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

        uint64_t getClockFrequency() const { return this->clockFrequency; }

        static SDL_Renderer* getRenderingContext() { return renderingContext; }
        static Logger& getLogger() { return logger; }
        // static InputHandler& getInputHandler() { return inputHandler; }
        static ResourceManager& getResourceManager() { return resourceManager; }

        // /**
        //  * @brief Get the explanation for the latest error that happened on this thread.
        //  * 
        //  * Only the latest error is returned, even if multiple errors happened in-between.
        //  * 
        //  * The error string is only changed when an error occurs, therefore
        //  * only call this function if an error occurs.
        //  * 
        //  * @return error explanation or empty string if error has been cleared with `ClearError()`,
        //  * do not free as it's ahandle to internally allocated data
        //  */
        // static const char* GetLatestError() { return latestError.c_str(); };
        // /**
        //  * @brief Get the explanation for the latest error that happened on the calling thread.
        //  * 
        //  * Only the latest error is returned, even if multiple errors happened in-between.
        //  * 
        //  * The error string is only changed when an error occurs, therefore
        //  * only call this function if an error occurs.
        //  * 
        //  * @return error explanation or empty string if error has been cleared with `ClearError()`,
        //  * do not free as it's ahandle to internally allocated data.
        //  */
        // static const string& GetLatestErrorString() { return latestError; }

        // /**
        //  * @brief Get the error code for the latest error that happened on the calling thread.
        //  * 
        //  * Only the latest error is returned, even if multiple errors happened in-between.
        //  * 
        //  * The error string is only changed when an error occurs, therefore
        //  * only call this function if an error occurs.
        //  * 
        //  * @return error code or Status::SUCCESS if error has been cleared with `ClearError()`.
        //  */
        // static Status GetLatestErrorCode() { return latestErrorCode; }

        // static void ClearError() {
        //     latestError = "";
        //     latestErrorCode = Status::SUCCESS;
        // }
    protected:
        struct ProgramFlags flags;

        /* static thread_local string latestError;
        static thread_local Status latestErrorCode; */

        uint64_t clockFrequency;

        //Video-related members
        SDL_Window* window;
        struct WindowParameters windowParameters;
        static SDL_Renderer* renderingContext;

        //Audio-related members
        struct AudioParameters audioParameters;
        
        static Logger logger;
        // static InputHandler inputHandler;
        static ResourceManager resourceManager;
};