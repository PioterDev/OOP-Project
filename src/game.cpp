#include "Game/Main/Game.hpp"
#include "Game/Main/GameObject.hpp"
#include "Game/Physics/PhysicalObject.hpp"
#include "Game/Render/UIElement.hpp"

using namespace Enums;
using namespace Structs;

MainRegistry Game::registry;
GameRenderer Game::renderer;
InputHandler Game::inputHandler;

Keymap testKeymap;


Game::~Game() {}

Status Game::init() {
    Status s = this->initSystems();
    if(s != Status::SUCCESS) return s;

    this->registry.init();

    return s;
}

void Game::run() {
    i64 start = 0, end = 0, delta = 0, overhead = 0, frameTime = 0;
    
    {
        KeyboardKey keys[3] = {KeyboardKey_LCTRL, KeyboardKey_LSHIFT, KeyboardKey_C};
        bool ifPressed[3] = {true, true, true};

        u64 c = testKeymap.registerKeyCombination(3, keys, ifPressed, false);
        std::function<void()> f = []() { Program::getLogger().println("Hello from Ctrl+Shift+C!"); };
        testKeymap.registerKeyCombinationCallback(c, std::move(f));

        keys[0] = KeyboardKey_LSHIFT;
        keys[1] = KeyboardKey_LCTRL;
        c = testKeymap.registerKeyCombination(3, keys, ifPressed, false);
        f = [](){ Program::getLogger().println("This is NOT Ctrl+Shift+C, this is Shift+Ctrl+C muahaha"); };
        testKeymap.registerKeyCombinationCallback(c, std::move(f));

        keys[0] = KeyboardKey_LCTRL;
        keys[1] = KeyboardKey_LCTRL;
        keys[2] = KeyboardKey_LCTRL;
        ifPressed[1] = false;
        c = testKeymap.registerKeyCombination(3, keys, ifPressed, false);
        f = [](){ Program::getLogger().println("Ctrl, release Ctrl, Ctrl again."); };
        testKeymap.registerKeyCombinationCallback(c, std::move(f));
     
        this->inputHandler.swapKeymap(&testKeymap);
    }

    ///Section for testing ///
    UIElement& obj = UIElement::createUIElement(MainRegistry::someObjectID, MainRegistry::gregTextureIndex);
    obj.setSizeOnScreen(400, 400).setPositionOnScreenCentered(640, 360);

    TextureHandle tex = Program::getResourceManager().createTextTexture(u"That will be 5€.", 0, MainRegistry::consolasFontIndex, Colors::RED, 480);
    UIElement& t = UIElement::createUIElement(MainRegistry::someObjectID, "Hello there", tex);
    Size s = Program::getResourceManager().getTextureOriginalSize(tex);
    t.setSizeOnScreen(s).setPositionOnScreen(0, 400);

    this->world.populateChunk({0, 0}, Blocks::cobblestone->getInstanceID());
    this->world.populateChunk({1, 1}, Blocks::cobblestone->getInstanceID());
    ///End of section for testing ///

    while(this->flags.running) {
        start = SDL_GetPerformanceCounter();
        frameTime = this->clockFrequency / this->renderer.fps;
        
        
        
        this->inputHandler.processInput(*this);
        if(!this->flags.paused) Likely this->renderer.renderInPlace(*this);
        


        end = SDL_GetPerformanceCounter();
        //Delta = Target frametime - Time elapsed - Overhead from previous frames
        delta = frameTime - (end - start) - overhead;
        delta = delta * (i64)1000 / (i64)this->clockFrequency;
        if(delta > 0) sleep(delta);

        end = SDL_GetPerformanceCounter();
        overhead = end - start - frameTime;
    }
}