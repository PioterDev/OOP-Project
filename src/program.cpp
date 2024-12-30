#include "Bindings.h"

#include "deus.hpp"
#include "program.hpp"

static constexpr u32 SDL_InitFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER;
static constexpr int Img_Init_Flags = IMG_INIT_JPG | IMG_INIT_PNG;
static constexpr int Mix_Init_Flags = MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_OPUS;

static constexpr Color defaultBackgroundColor = {80, 80, 80, 255};

// Program::InputHandler Program::inputHandler;
ResourceManager     Program::resourceManager;
Logger              Program::logger;
SDL_Renderer*       Program::renderingContext = nullptr;
u64                 Program::clockFrequency;
const uint8_t*      Program::keyboardState;

Status Program::initSystems() {
    const char* platform = SDL_GetPlatform();
    if(!strcmp(platform, "Windows")) {
        SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");
        // SDL_SetHint(SDL_HINT_AUDIO_INCLUDE_MONITORS, "1");
    }
    if(Program::logger.init("./log/latest.log") != Status::SUCCESS) return Status::LOGGER_FAILURE;
    Program::logger.info(platform);
    if(SDL_Init(SDL_InitFlags)) {
        Program::logger.fatal("SDL initialization failed: ", SDL_GetError());
        return Status::SDL_INIT_FAILURE;
    }
    this->flags.SDL_Initialized = true;

    if(!(IMG_Init(Img_Init_Flags) & Img_Init_Flags)) {
        Program::logger.fatal("SDL_image initialization failed: ", IMG_GetError());
        return Status::SDL_IMAGE_INIT_FAILURE;
    }
    this->flags.SDL_Image_Initialized = true;
    
    if(!(Mix_Init(Mix_Init_Flags) & Mix_Init_Flags)) {
        Program::logger.error("SDL_mixer initialization failed: ", Mix_GetError());
        return Status::SDL_MIXER_INIT_FAILURE;
    }
    else this->flags.SDL_Mixer_Initialized = true;

    u8 allocatedAudioChannels = (u8)Mix_AllocateChannels(this->audioParameters.audioChannelAmount);
    if(allocatedAudioChannels != this->audioParameters.audioChannelAmount) {
        Program::logger.error(
            "Failed to reserve ",
            (int)this->audioParameters.audioChannelAmount,
            " audio channels, will use ", (int)allocatedAudioChannels
        );
        this->audioParameters.audioChannelAmount = allocatedAudioChannels;
    }

    if(Mix_OpenAudioDevice(
        48000, MIX_DEFAULT_FORMAT, 2,
        2048, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE
    )) {
        //not necessarily fatal, the program will run fine without audio
        Program::logger.error(
            "Audio device opening failed: ",
            Mix_GetError(),
            "; Will continue without the audio subsystem"
        );
    }
    else {
        this->flags.canPlaySound = true;
    }


    if(TTF_Init() != 0) {
        Program::logger.fatal("SDL_ttf initialization failed: ", TTF_GetError());
        return Status::SDL_TTF_FAILURE;
    }
    this->flags.SDL_TTF_Initalized = true;
    //TODO: SDL_TTF
    
    this->window = SDL_CreateWindow(
        windowParameters.name.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowParameters.size.width,
        windowParameters.size.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if(this->window == nullptr) {
        logger.fatal("Window creation failed: ", SDL_GetError());
        return Status::SDL_WINDOW_CREATION_FAILURE;
    }

    Program::renderingContext = SDL_CreateRenderer(this->window, -1, 0);
    if(Program::renderingContext == nullptr) {
        logger.fatal("Renderer creation failed: ", SDL_GetError());
        return Status::SDL_RENDERER_CREATION_FAILURE;
    }

    if(Program::resourceManager.init() != Status::SUCCESS) {
        Program::logger.fatal("Resource manager initialization failed");
        return Status::FALLBACK_TEXTURE_CREATION_FAILURE;
    }
    
    srand(time(nullptr));
    this->flags.running = true;
    this->flags.paused = false;
    
    Program::clockFrequency = SDL_GetPerformanceFrequency();
    Program::keyboardState = SDL_GetKeyboardState((int*)&this->numberOfKeyboardKeys);
    this->backgroundColor = defaultBackgroundColor;
    
    return Status::SUCCESS;
}

Program::~Program() {
    this->resourceManager.shutdown();
    if(this->renderingContext != nullptr) SDL_DestroyRenderer(Program::renderingContext);
    if(this->window != nullptr) SDL_DestroyWindow(this->window);
    if(this->flags.SDL_TTF_Initalized) TTF_Quit();
    if(this->flags.SDL_Mixer_Initialized) {
        Mix_CloseAudio();
        Mix_Quit();
    }
    if(this->flags.SDL_Image_Initialized) IMG_Quit();
    if(this->flags.SDL_Initialized) SDL_Quit();
}