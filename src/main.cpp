#include "Bindings.h"

#include <SDL_messagebox.h>

#include "Game/Main/Game.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    Game game;
    Status status = game.init();
    if(status != Status::SUCCESS) Unlikely {
        char message[128];
        snprintf(
            message, sizeof(message), 
            "The program could not start and exited with code %d."
            " For further information, please contact the developer.", 
            static_cast<int>(status)
        );
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "Could not start", message, nullptr
        );
        return static_cast<int>(status);
    }
    
    try {
        game.run();
    }
    catch(int code) {
        //TODO: finish this
    }
    catch(const std::bad_alloc& e) {
        char message[128];
        snprintf(
            message, sizeof(message),
            "The program encountered a fatal exception: OutOfMemory and"
            "cannot continue normally."
        );
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "No memory",
            message, game.getWindow()
        );
        return static_cast<int>(Status::ALLOC_FAILURE);
    }


    return 0;
}