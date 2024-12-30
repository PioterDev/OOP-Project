#pragma once

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
#include "Game/Main/MainRegistry.hpp"
#include "Game/World.hpp"
#include "program.hpp"
#include "resources.hpp"
#include "input.hpp"

class Game : public Program {
    friend class InputHandler;

    private:
        static MainRegistry registry;
        static GameRenderer renderer;
        static InputHandler inputHandler;

        World world;

    public:
        Game() : world(1) /* TODO: change this later */ {
            
        }
        ~Game();
        
        static MainRegistry& getRegistry() { return registry; }
        static GameRenderer& getRenderer() { return renderer; }
        static InputHandler& getInputHandler() { return inputHandler; }
        NoDiscard Status init();
        void run();

        static u64 getTimeSinceLastFrame() { return renderer.getTimeSinceLastFrame(); }

        World& getWorld() { return this->world; }
};

#endif