#include "Bindings.h"

#include "logging.hpp"
#include "program.hpp"
#include "game.hpp"

int main(Unused int argc, Unused char** argv) {
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

    game.run();

    return 0;
}