#ifndef GAME_HPP
/**
 * @file game.hpp
 * @author PioterDev
 * @brief 
 * @version 0.1
 * @date 2024-10-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#define GAME_HPP

#include "Game/GameRenderer.hpp"
#include "Game/GameRegistry.hpp"
#include "program.hpp"
#include "resources.hpp"
#include "input.hpp"

class Game : public Program {
    friend class InputHandler;

    private:
        static GameRegistry registry;
        static GameRenderer renderer;
        static InputHandler inputHandler;
    public:
        ~Game();
        static GameRegistry& getRegistry() { return registry; }
        static GameRenderer& getRenderer() { return renderer; }
        static InputHandler& getInputHandler() { return inputHandler; }
        NoDiscard Status init();
        void run();
};

#endif