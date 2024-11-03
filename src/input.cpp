#include <SDL.h>

#include "input.hpp"
#include "game.hpp"

const char keynames[179][32] = {
    //0
    "Unknown",   //0
    //1-5
    "Enter",    //13
    "Escape",    //27
    "Backspace", //8
    "Tab",       //9
    "Space",     //32
    //6-18
    "Caps Lock",
    "F1", "F2", "F3", "F4", "F5", "F6", //1073741882-1073741893
    "F7", "F8", "F9", "F10", "F11", "F12", 
    //19-23
    "Print Screen", //1073741894
    "Scroll Lock",
    "Pause/Break",
    "Insert",
    "Home",
    "Page Up", //1073741899
    //25
    "Delete", //127
    //26-31
    "End", //1073741901
    "Page Down",
    "Right arrow", "Left arrow", "Down arrow", "Up arrow", //1073741906
    //32-48
    "Num Lock", //1073741907
    "Keypad /", "Keypad *", "Keypad -", "Keypad +", "Keypad Enter",
    "Keypad 1", "Keypad 2", "Keypad 3", "Keypad 4", "Keypad 5",
    "Keypad 6", "Keypad 7", "Keypad 8", "Keypad 9", "Keypad 0",
    "Keypad .", //1073741923
    //49-78, the weird ones start here
    "Application", //1073741925
    "Power",
    "Keypad =",
    "F13", "F14", "F15", "F16", "F17", "F18", 
    "F19", "F20", "F21", "F22", "F23", "F24",
    "Execute",
    "Help", "Menu", "Select", "Stop", "Repeat",
    "Undo", "Cut", "Copy", "Paste", "Find",
    "Volume up", "Volume down", "Keypad ,",
    "Keypad =400???", //1073741958
    //79-90
    "Alt erase?", //1073741977
    "Sysrq",
    "Cancel",
    "Clear",
    "Prior",
    "Backspace the 2nd",
    "Separator ? ? ?",
    "Out? Out of what?",
    "Oper? Operation?",
    "Clear again? Why?!",
    "CrSel (whatever this is...)",
    "ExSel (whatever this is...)", //1073741988
    //91-136
    "Keypad 00", //1073742000
    "Keypad 000",
    "Thousands separator? Fancy!",
    "Decimal separator? Calc gaming",
    "key unit...",
    "Currency sub-unit? HUH",
    "Keypad (", "Keypad )", "Keypad {", "Keypad }",
    "Keypad Tab",
    "Keypad Backspace",
    "Keypad A", "Keypad B", "Keypad C",
    "Keypad D", "Keypad E", "Keypad F",
    "Keypad XOR",
    "Keypad Power",
    "Keypad %",
    "Keypad <", "Keypad >",
    "Keypad &", "Keypad &&",
    "Keypad |", "Keypad ||",
    "Keypad :", "Keypad #",
    "Keypad Space",
    "Keypad @", "Keypad !",
    "Keypad MemStore, damn", "Keypad MemRecall", "Keypad MemClear, bruh",
    "Keypad MemAdd", "Keypad MemSubtract",
    "Keypad MemMultiply", "Keypad MemDivide",
    "Keypad +/-",
    "Keypad Clear", "Keypad Clear entry",
    "Keypad Binary", "Keypad Octal",
    "Keypad Decimal", "Keypad Hexadecimal", //1073742045
    //137-144
    "Left Control", "Left Shift", "Left Alt", "Left GUI", //1073742048
    "Right Control", "Right Shift", "Right Alt", "Right GUI", //1073742055
    //145
    "Mode key", //1073742081
    //146-162
    "Audio next", //1073742082
    "Audio previous",
    "Audio stop",
    "Audio play",
    "Audio mute",
    "Media select",
    "Web",
    "Mail",
    "Calculator",
    "Computer",
    "American Search",
    "American Home",
    "American back",
    "American forward",
    "American stop",
    "Americal refresh",
    "American bookmarks", //1073742098
    //163-172
    "Brightness down", //1073742099
    "Brightness up",
    "Toggle display",
    "Toggle illumination",
    "Illumination down",
    "Illumination up",
    "Eject",
    "Sleep",
    "App 1",
    "App 2", //1073742108
    //173-174
    "Audio rewind", //1073742109
    "Audio fast-forward", //1073742110
    //175-178
    "Left soft key", //1073742111
    "Right soft key",
    "Call",
    "End call" //1073742114
};

int getKeystringIndex(SDL_Keycode key) {
    if((key >= 33 && key <= 64) || (key >= 91 && key <= 126)) return -1; //ASCII character
    
    int index = 0;
    //first check the irregular ones
    switch(key) {
        case SDLK_RETURN:
            index = 1;
            break;
        case SDLK_ESCAPE:
            index = 2;
            break;
        case SDLK_BACKSPACE:
            index = 3;
            break;
        case SDLK_TAB:
            index = 4;
            break;
        case SDLK_SPACE:
            index = 5;
            break;
        case SDLK_DELETE:
            index = 25;
            break;
        case SDLK_MODE:
            index = 145;
            break;
        case SDLK_AUDIOREWIND:
            index = 173;
            break;
        case SDLK_AUDIOFASTFORWARD:
            index = 174;
            break;
    }
    //if the key is not there, check the continous ones
    if(index == 0) {
        if(key >= SDLK_CAPSLOCK && key <= SDLK_F12)                     index = key - SDLK_CAPSLOCK + 6;
        else if(key >= SDLK_PRINTSCREEN && key <= SDLK_PAGEUP)          index = key - SDLK_PRINTSCREEN + 19;
        else if(key >= SDLK_END && key <= SDLK_UP)                      index = key - SDLK_END + 26;
        else if(key >= SDLK_NUMLOCKCLEAR && key <= SDLK_KP_PERIOD)      index = key - SDLK_NUMLOCKCLEAR + 32;
        else if(key >= SDLK_APPLICATION && key <= SDLK_KP_EQUALSAS400)  index = key - SDLK_APPLICATION + 49;
        else if(key >= SDLK_ALTERASE && key <= SDLK_EXSEL)              index = key - SDLK_ALTERASE + 79;
        else if(key >= SDLK_KP_00 && key <= SDLK_KP_HEXADECIMAL)        index = key - SDLK_KP_00 + 91;
        else if(key >= SDLK_LCTRL && key <= SDLK_RGUI)                  index = key - SDLK_LCTRL + 137;
        else if(key >= SDLK_AUDIONEXT && key <= SDLK_AC_BOOKMARKS)      index = key - SDLK_AUDIONEXT + 146;
        else if(key >= SDLK_BRIGHTNESSDOWN && key <= SDLK_APP2)         index = key - SDLK_BRIGHTNESSDOWN + 163;
        else if(key >= SDLK_SOFTLEFT && key <= SDLK_ENDCALL)            index = key - SDLK_SOFTLEFT + 175;
    }
    return index;
}

void InputHandler::processInput(Game* game) {
    //Aliases
    #define latestEvent latestEvents.buffer[latestEvents.index]
    constexpr size_t bufferSize = sizeof(latestEvents.buffer) / sizeof(SDL_Event);

    while(SDL_PollEvent(&latestEvent)) {
        switch(latestEvent.type) {
            case SDL_QUIT:
                game->flags.running = false;
                // game->renderer.status = ThreadStatus::STOP;
                break;
            case SDL_WINDOWEVENT:
                switch(latestEvent.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        //reload all textures
                        game->windowParameters.size.width = latestEvent.window.data1;
                        game->windowParameters.size.height = latestEvent.window.data2;
                        Program::getLogger().info(
                            "Resized window to ",
                            latestEvent.window.data1,
                            "x",
                            latestEvent.window.data2
                        );
                        break;
                    case SDL_WINDOWEVENT_MINIMIZED: 
                        break;
                }
                break;
                
            case SDL_KEYDOWN: {
                break;
            }

            case SDL_KEYUP: {
                break;
            }

            case SDL_MOUSEWHEEL: {
                if(game->renderer.scalingFactor < 0.05 && latestEvent.wheel.y < 0) break;
                game->renderer.scalingFactor += (double)latestEvent.wheel.y * 0.01;

                break;
            }
            case SDL_MOUSEMOTION: {
                break;
            }

        }
        latestEvents.index = (latestEvents.index + 1) % bufferSize;
    }
    
    if(Program::keyboardState[SDL_SCANCODE_RIGHT])  game->renderer.moveCamera(1,  0);
    if(Program::keyboardState[SDL_SCANCODE_LEFT])   game->renderer.moveCamera(-1, 0);
    if(Program::keyboardState[SDL_SCANCODE_DOWN])   game->renderer.moveCamera(0,  1);
    if(Program::keyboardState[SDL_SCANCODE_UP])     game->renderer.moveCamera(0, -1);

    #undef latestEvent
}