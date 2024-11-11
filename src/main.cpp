#include "Bindings.h"

#include <SDL_messagebox.h>

#include "game.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    Game game;
    Status status = game.init();
    if(status != Status::SUCCESS) {
        char message[128];
        snprintf(
            message, sizeof(message), 
            "The program could not start and exited with the code %d."
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


    return 0;
}