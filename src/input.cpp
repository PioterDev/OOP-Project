#include <SDL.h>

#include "input.hpp"
#include "Game/Main/Game.hpp"

using namespace Enums;

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
    "Help", "Menu", "Select", "Stop", "Redo",
    "Undo", "Cut", "Copy", "Paste", "Find",
    "Volume up", "Volume down", "Keypad ,",
    "Keypad = (AS/400)", //1073741958
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
    "AC Search",
    "AC Home",
    "AC back",
    "AC forward",
    "AC stop",
    "AC refresh",
    "AC bookmarks", //1073742098
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

const char keyboardKeynames[KeyboardKey_AMOUNT][32] = {
    "Unknown",
    
    "A", "B", "C", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
    "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",

    "Enter", "Esc", "Backspace", "Tab", "Spacebar", 
    "-", "=", "{", "}", "\\", "\\ (nonexistent in practise)",
    ";", "'", "`", ",", ".", "/", "Caps Lock",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",

    "Print Screen", "Scroll Lock", "Pause",
    "Insert", "Home", "Page Up",
    "Delete", "End", "Page Down",

    "Right Arrow", "Left Arrow", "Down Arrow", "Up Arrow",
    
    "Numlock", "Keypad /", "Keypad *", "Keypad -", "Keypad +", "Keypad Enter",
    "Keypad 1", "Keypad 2", "Keypad 3", "Keypad 4", "Keypad 5",
    "Keypad 6", "Keypad 7", "Keypad 8", "Keypad 9", "Keypad 0",
    "Keypad .", "Some weird ISO key",
    
    "Application", "Power", 
    
    "Keypad =",

    "F13", "F14", "F15", "F16", "F17", "F18",
    "F19", "F20", "F21", "F22", "F23", "F24",

    "Execute (huzzah!)", "Help", "Menu", "Select",
    "Stop", "Again", "Undo", "Cut", "Copy", "Paste", "Find",
    "Mute", "Volume Up", "Volume Down", 
    
    "Keypad ,", "Keypad = (AS400), wtf is this",
    "International 1", "International 2", "International 3", 
    "International 4", "International 5", "International 6",
    "International 7", "International 8", "International 9",
    
    "Language 1", "Language 2", "Language 3",
    "Language 4", "Language 5", "Language 6",
    "Language 7", "Language 8", "Language 9",

    "AltErase", "SysReq", "Cancel (Twitter)", "Prior", "Return", "Separator",
    "Out", "Oper", "Clear / Again", "CrSel", "ExSel",

    "Keypad 00", "Keypad 000",
    "Thousands separator", "Decimal separator", "Currency unit", "Currency subunit",
    "Keypad (", "Keypad )", "Keypad {", "Keypad }",
    "Keypad Tab", "Keypad Backspace",
    "Keypad A", "Keypad B", "Keypad C", "Keypad D", "Keypad E", "Keypad F",
    "Keypad XOR", "Keypad ^", "Keypad %",
    "Keypad <", "Keypad >",
    "Keypad &", "Keypad &&", "Keypad |", "Keypad ||",
    "Keypad :", "Keypad #", "Keypad Space", "Keypad @", "Keypad !",
    "Keypad MemStore", "Keypad MemRecall",
    "Keypad MemAdd", "Keypad MemSubtract", "Keypad MemMultiply", "Keypad MemDivide",
    "Keypad +/-", "Keypad Clear", "Keypad ClearEntry",
    "Keypad Binary", "Keypad Octal", "Keypad Hexadecimal",
    "Left Ctrl", "Left Shift", "Left Alt", "Left GUI",
    "Right Ctrl", "Right Shift", "Right Alt", "Right GUI",
    
    "ModeSwitch",
    
    "Audio Next", "Audio Previous", "Audio Stop", "Audio Play", "Audio Mute",
    "MediaSelect",
    "WWW", "Mail", "Calculator", "Computer",
    
    "AC Search", "AC Home", "AC Back", "AC Forward",
    "AC Stop", "AC Refresh", "AC Bookmarks",

    "Brightness Down", "Brightness Up",
    "Display Switch",
    "Keyboard Illumination Toggle", "Keyboard Illumination Down", "Keyboard Illumination Up",

    "Eject", "Sleep", "App 1", "App 2",

    "Audio Rewind", "Audio Fast Fast-forward",
    
    "Soft Left", "Soft Right", "Call", "End Call"
};

int getKeystringIndex(SDL_Keycode key) {
    if((key >= 33 && key <= 64) || (key >= 91 && key <= 126)) return -1; //ASCII character
    
    int index = 0;
    //first check the irregular ones
    switch(key) {
        case SDLK_RETURN:           index = 1; break;
        case SDLK_ESCAPE:           index = 2; break;
        case SDLK_BACKSPACE:        index = 3; break;
        case SDLK_TAB:              index = 4; break;
        case SDLK_SPACE:            index = 5; break;
        case SDLK_DELETE:           index = 25; break;
        case SDLK_MODE:             index = 145; break;
        case SDLK_AUDIOREWIND:      index = 173; break;
        case SDLK_AUDIOFASTFORWARD: index = 174; break;
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

KeyboardKey getKeyFromScancode(SDL_Scancode keycode) {
    int key = (int)keycode;
    if(key >= SDL_SCANCODE_A && key <= SDL_SCANCODE_VOLUMEDOWN)
        return (KeyboardKey)(key - ((int)SDL_SCANCODE_A - (int)KeyboardKey_A));
    else if(key >= SDL_SCANCODE_KP_COMMA && key <= SDL_SCANCODE_EXSEL)
        return (KeyboardKey)(key - ((int)SDL_SCANCODE_KP_COMMA - (int)KeyboardKey_KP_COMMA));
    else if(key >= SDL_SCANCODE_KP_00 && key <= SDL_SCANCODE_KP_HEXADECIMAL)
        return (KeyboardKey)(key - ((int)SDL_SCANCODE_KP_00 - (int)KeyboardKey_KP_00));
    else if(key >= SDL_SCANCODE_LCTRL && key <= SDL_SCANCODE_RGUI)
        return (KeyboardKey)(key - ((int)SDL_SCANCODE_LCTRL - (int)KeyboardKey_LCTRL));
    else if(key >= SDL_SCANCODE_MODE && key <= SDL_SCANCODE_ENDCALL)
        return (KeyboardKey)(key - ((int)SDL_SCANCODE_MODE - (int)KeyboardKey_MODE));
    else return KeyboardKey_UNKNOWN;
}

const char* getKeyName(Enums::KeyboardKey key) {
    return static_cast<u32>(key) < Enums::KeyboardKey_AMOUNT
        ? keyboardKeynames[static_cast<u32>(key)]
        : "";
}



constexpr u64 Keymap::getKeyCombinationBitmask(
    const u8 numberOfKeys, const KeyboardKey* keyArr,
    const bool* isPressedArr, const bool hold
) const noexcept {
    assert(keyArr);
    assert(isPressedArr);
    if(numberOfKeys > 7) return 0;

    u64 combination = 0;
    for(u8 i = 0; i < numberOfKeys; i++) {
        combination |= ((u64)keyArr[numberOfKeys - 1 - i]) << 8 * i;
        combination |= ((u64)(isPressedArr[numberOfKeys - 1 - i])) << (i + 56);
    }
    combination |= hold ? (u64)1 << 63 : 0;
    return combination;
}

u64 Keymap::registerKeyCombination(
    const u8 numberOfKeys, const KeyboardKey* keyArr,
    const bool* isPressedArr, const bool hold
) noexcept {
    u64 combination = this->getKeyCombinationBitmask(numberOfKeys, keyArr, isPressedArr, hold);
    if(!combination) return 0;

    try {
        this->keybinds.addInPlace(combination);
    }
    catch(const std::bad_alloc& e) {
        return 0;
    }
    return combination;
}

bool Keymap::registerKeyCombinationCallback(
    const u64 combination, std::function<void()>&& callback
) noexcept {
    KeyCombinationBinding* comb = this->findKeyCombination(combination);
    if(comb == nullptr) return false;
    return comb->callbacks.subscribe(std::move(callback));
}

bool Keymap::isKeyCombinationHold(const u64 combination) const noexcept {
    KeyCombinationBinding* comb = this->findKeyCombination(combination);
    if(comb == nullptr) return false;
    //whether the combination's most significant bit is set
    return comb->combination == ((u64)1 << 63);
}

EventNotifier<1>* Keymap::getKeyCombinationCallbacks(const u64 combination) const noexcept {
    KeyCombinationBinding* comb = this->findKeyCombination(combination);
    if(comb == nullptr) return nullptr;
    return &comb->callbacks;
}


void InputHandler::processInput(Game& game) {
    //Aliases
    #define latestEvent latestEvents.buffer[latestEvents.index]
    constexpr size_t bufferSize = sizeof(latestEvents.buffer) / sizeof(SDL_Event);

    //It's actually possible to register a keybind for
    //released keys (for example, releasing W
    //and not pressing anything for some time).
    //Handling such keybinds is tricky as SDL doesn't continously
    //emit SDL_KEYUP events for keys that are not pressed
    //(not entirely true), unlike pressed ones.
    //This necessitates checking if *any* key was
    //pressed. If not, then and only then check for such keybinds.
    //Hence, this boolean.
    bool anyKeyPressed = false;

    while(SDL_PollEvent(&latestEvent)) {
        switch(latestEvent.type) {
            case SDL_QUIT:
                game.flags.running = false;
                break;
            case SDL_WINDOWEVENT:
                switch(latestEvent.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        game.windowParameters.size.width = latestEvent.window.data1;
                        game.windowParameters.size.height = latestEvent.window.data2;
                        Program::getLogger().debug(
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
                anyKeyPressed = true;

                /**
                 * @brief What is done here is a software
                 * implementation of a 7-byte shift
                 * register + special masking of the
                 * most significant byte.
                 * 
                 * The most significant byte is used to store whether the key was
                 * pressed (1) or released (0). As there are 7 lower bytes used,
                 * the most significant bit is not used to store information about
                 * any key, but whether the combination is a "held"
                 * combination or a "one-time" one.
                 * 
                 * The remaining lower 7 bytes store the encoded value of a key
                 * from the KeyboardKey enum.
                 * 
                 * This all is used to efficiently implement complex keybinds
                 * like Ctrl+Shift+C, (sidenote: Shift+Ctrl+C *is* a distinct keybind here)
                 * as well as Ctrl+Shift+[release]Ctrl+C, which is not possible
                 * in most applications.
                 */
                u64 latestPressed = (u64)getKeyFromScancode(latestEvent.key.keysym.scancode);
                //If the latest pressed key recorded is NOT the same as
                //the one emitted in this SDL_Event,
                //update the current key combination
                //and check for registered keybinds.
                if(!(
                    (this->currentKeyCombination & (u64)0xFF) == latestPressed &&
                    this->currentKeyCombination & ((u64)1 << 56)
                )) {
                    //The most significant byte stores whether
                    //the key was pressed/released
                    u64 keyStates = this->currentKeyCombination & ((u64)0xFF << 56);
                    //Left shift by 1, make the least significant bit 1
                    //(because the key was pressed)
                    //and remove the most significant bit
                    //(since maximum 7 keys are supported)
                    keyStates = ((keyStates << 1) | ((u64)1 << 56)) & ((u64)0x7F << 56);
                    //Left shift by 1 BYTE, ignore the most significant byte,
                    //insert the latest pressed key and keyStates back
                    this->currentKeyCombination =
                        ((this->currentKeyCombination << 8) & 0xFFFFFFFFFFFFFF) |
                        latestPressed |
                        keyStates;
                    
                    /* Program::getLogger().debug(std::bitset<8>(keyStates >> 56), ' ',
                        ((this->currentKeyCombination & 0xFF000000000000) >> 48), ' ',
                        ((this->currentKeyCombination & 0xFF0000000000) >> 40), ' ',
                        ((this->currentKeyCombination & 0xFF00000000) >> 32), ' ',
                        ((this->currentKeyCombination & 0xFF000000) >> 24), ' ',
                        ((this->currentKeyCombination & 0xFF0000) >> 16), ' ',
                        ((this->currentKeyCombination & 0xFF00) >> 8), ' ',
                        ((this->currentKeyCombination & 0xFF) >> 0)
                    ); */

                    //First check for "one-time" keybinds...
                    this->checkForKeyCombination(this->currentKeyCombination);
                    //...then for held ones.
                    //This makes it possible to create distinct keybinds
                    //for both "one-time" presses events as well as
                    //"held" ones without collision.
                    this->checkForKeyCombination(this->currentKeyCombination | ((u64)1 << 63));
                }
                //If the latest pressed key recorded is the same as
                //the one emitted in this SDL_Event,
                //ignore it and check whether there are keybinds
                //that should be repeated every SDL_KEYDOWN event.
                //SDL will continously produce events for held keys,
                //so the current key combination should stay the same.
                else {
                    //Check for held combinations, i.e. if the user
                    //pressed Ctrl, Shift, holds C and there is a keybind
                    //registered to be repeated while Ctrl+Shift+C is pressed,
                    //then its callbacks should be invoked.
                    this->checkForKeyCombination(this->currentKeyCombination | ((u64)1 << 63));
                }

                if(latestEvent.key.keysym.sym == SDLK_ESCAPE) {
                    if(game.flags.paused) game.flags.paused = false;
                    else game.flags.paused = true;
                }
                break;
            }

            case SDL_KEYUP: {
                u64 latestReleased = (u64)getKeyFromScancode(latestEvent.key.keysym.scancode);
                //Checking if the latest released key recorded
                //is the same as the one emitted by this SDL_event doesn't
                //make sense in contrast to pressed ones because
                //there are no events emitted for continously
                //released keys (I mean, duh).
                //Update: it actually DOES make sense, as for some reason
                //SDL sends duplicate SDL_KEYUP events. I don't know why,
                //but I guess they should be ignored.

                if(!(
                    (this->currentKeyCombination & (u64)0xFF) == latestReleased &&
                    !(this->currentKeyCombination & ((u64)1 << 56))
                )) {
                    //Same logic as in SDL_KEYDOWN, except this time
                    //the least significant bit is not set.
                    u64 keyStates = this->currentKeyCombination & ((u64)0xFF << 56);
                    keyStates = (keyStates << 1) & ((u64)0x7F << 56);
                    this->currentKeyCombination =
                        ((this->currentKeyCombination << 8) & 0xFFFFFFFFFFFFFF) |
                        latestReleased |
                        keyStates;

                    this->checkForKeyCombination(this->currentKeyCombination);
                    this->checkForKeyCombination(this->currentKeyCombination | ((u64)1 << 63));
                }
                //Same deal as with SDL_KEYDOWN
                else {
                    this->checkForKeyCombination(this->currentKeyCombination | ((u64)1 << 63));
                }

                break;
            }

            case SDL_MOUSEWHEEL: {
                if(game.renderer.scalingFactor < 0.05 && latestEvent.wheel.y < 0) break;
                if(latestEvent.wheel.y < 0) {
                    game.renderer.scalingFactor -= (double)latestEvent.wheel.preciseY * (double)latestEvent.wheel.preciseY * 0.01;
                }
                else {
                    game.renderer.scalingFactor += (double)latestEvent.wheel.preciseY * (double)latestEvent.wheel.preciseY * 0.01;
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                break;
            }
        }
        latestEvents.index = (latestEvents.index + 1) % bufferSize;
    }
    
    
    if(!anyKeyPressed) { //handle "while released" keybinds
        this->checkForKeyCombination(this->currentKeyCombination | ((u64)1 << 63));
    }

    game.updateMouse();
    
    const u8* keyboardState = Program::getKeyboardState();

    if(keyboardState[SDL_SCANCODE_RIGHT])  game.renderer.moveCamera(1,  0);
    if(keyboardState[SDL_SCANCODE_LEFT])   game.renderer.moveCamera(-1, 0);
    if(keyboardState[SDL_SCANCODE_DOWN])   game.renderer.moveCamera(0,  1);
    if(keyboardState[SDL_SCANCODE_UP])     game.renderer.moveCamera(0, -1);

    #undef latestEvent
}

void InputHandler::checkForKeyCombination(u64 comb) {
    for(int i = 0; i < 7; i++) {
        if(this->currentKeymap == nullptr) break; //no keymap, no keybinds
        EventNotifier<1>* callbacks = 
        this->currentKeymap->getKeyCombinationCallbacks(
            comb & InputHandler::keyCombinationMasks[i]
        );
        if(callbacks) {
            callbacks->notifyAll();
            //To make keybinds non-cascadable
            //(i.e. pressing Shift+C should 
            //NOT trigger a keybind for C).
            break;
        }
    }
}