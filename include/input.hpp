#pragma once

#include <SDL_events.h>

#include "deus.hpp"

#define isFunctionalKey(key) \
    ((key) >= SDLK_LCTRL && (key) <= SDLK_RGUI) || \
    ((key) >= SDLK_AUDIONEXT && (key) <= SDLK_AUDIOMUTE) || \
    ((key) >= SDLK_APPLICATION && (key) <= SDLK_KP_EQUALSAS400) || \
    ((key) >= SDLK_F1 && (key) <= SDLK_F12) || \
    ((key) >= SDLK_BRIGHTNESSDOWN && (key) <= SDLK_APP2) || \
    ((key) == SDLK_TAB) || ((key) == SDLK_ESCAPE) || \
    ((key) == SDLK_RETURN) || ((key) == SDLK_BACKSPACE) || \
    ((key) >= SDLK_PRINTSCREEN && (key) <= SDLK_PAUSE) || \
    ((key) == SDLK_NUMLOCKCLEAR) || ((key) == SDLK_CAPSLOCK)

struct EventBuffer {
    //A circular buffer of latest external events
    SDL_Event buffer[64];
    //Current event index
    u32 index = 0;
};

/**
 * @brief Keynames for every key
 * recognizable by SDL, apart from ASCII.
 */
extern const char keynames[179][32];

/**
 * @brief Function converting SDL keycodes to their corresponding string indices in the `keynames` table.
 * 
 * @param key SDL keycode
 * @return index in `keynames`, -1 if it's an ASCII character
 */
int getKeystringIndex(SDL_Keycode key);

class Game;

class InputHandler {
    friend class Game;

    private:
        struct EventBuffer latestEvents;
    public:
        const SDL_Event& getLatestEvent() const { 
            return latestEvents.buffer[latestEvents.index];
        }
        
        void processInput(Game& game);
};