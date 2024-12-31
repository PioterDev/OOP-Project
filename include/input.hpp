#pragma once

#include <SDL_events.h>

#include "deus.hpp"
#include "EventNotifier.hpp"
#include "DSA/SortedVector.hpp"



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
 * @brief Keynames for every physical key
 * recognizable by SDL, mapped to Enums::KeyboardKey enum.
 */
extern const char keyboardKeynames[Enums::KeyboardKey_AMOUNT][32];

/**
 * @brief Function converting SDL keycodes to their corresponding string indices in the `keynames` table.
 * 
 * @param key SDL keycode
 * @return index in `keynames`, -1 if it's an ASCII character
 */
int getKeystringIndex(SDL_Keycode key);

/**
 * @brief Convert SDL_Scancode version of the keycode to Enums::KeyboardKey.
 * 
 * @param keycode SDL_Scancode-mapped keycode
 * @return Enums::KeyboardKey-mapped keycode
 */
Enums::KeyboardKey getKeyFromScancode(SDL_Scancode keycode);

/**
 * @brief Get a human-readable version of the key.
 * 
 * @param key keycode, mapped to Enums::KeyboardKey
 * @return a string literal containing the key's name or "" on invalid input
 */
const char* getKeyName(Enums::KeyboardKey key);



/**
 * @brief A class for encapsulating key combinations
 * and their corresponding functionality.
 * 
 * The combination is stored as
 * a sequence of 8 bytes (up to 7 keycodes)
 * (who the hell presses more than 7 keys at a time?!)
 * with the most recend keycode as the least significant byte
 * (little-endian).
 * Additionally, the most significant byte is used to determine
 * whether consequent keys are pressed or not. This allows for
 * creating not only order-dependent keybinds, but also
 * keybinds where the user releases a key. 
 * The most significant bit is used
 * 
 * For example, Ctrl+Shift+C would be stored as follows:
 * 
 * (Keycode_C) | (Keycode_SHIFT << 8) | (Keycode_CTRL << 16) | (0x07 << 56).
 * 
 * For simplicity, key combinations can be registered with 
 * `registerKeyCombination(...)`
 */
class Keymap {
    private:
        class KeyCombinationBinding {
            public:
                u64 combination;
                /**
                 * @brief A nifty little class to store callback functions
                 * on the user pressing this combination.
                 * 
                 * We can assume that, usually, only 1 function
                 * will be bound for some key combination (I mean,
                 * this system is very flexible).
                 */
                EventNotifier<1> callbacks;

                KeyCombinationBinding() = default;
                KeyCombinationBinding(u64 combination) : combination(combination) {}
                ~KeyCombinationBinding() = default;
                KeyCombinationBinding(const KeyCombinationBinding& other) = delete;
                KeyCombinationBinding(KeyCombinationBinding&& other) noexcept
                    : combination(other.combination), callbacks(std::move(other.callbacks)) {
                    other.combination = 0;
                }
                KeyCombinationBinding& operator=(const KeyCombinationBinding& other) = delete;
                KeyCombinationBinding& operator=(KeyCombinationBinding&& other) {
                    this->combination = other.combination;
                    this->callbacks = std::move(other.callbacks);

                    other.combination = 0;
                    
                    return *this;
                }

                bool operator< (const KeyCombinationBinding& other) const noexcept { return this->combination <  other.combination; }
                bool operator> (const KeyCombinationBinding& other) const noexcept { return this->combination >  other.combination; }
                bool operator<=(const KeyCombinationBinding& other) const noexcept { return this->combination <= other.combination; }
                bool operator>=(const KeyCombinationBinding& other) const noexcept { return this->combination >= other.combination; }
                bool operator==(const KeyCombinationBinding& other) const noexcept { return this->combination == other.combination; }
                bool operator!=(const KeyCombinationBinding& other) const noexcept { return this->combination != other.combination; }
        };
        //128 seems like a reasonable amount of preallocated
        //keybinds. In a very rare scenario that the keymap
        //uses more than that, it will simply allocate
        //more on the heap. But if not, then this reduces
        //the amount of pointer dereferences by a whopping 1.
        static constexpr const u64 staticCombinationCapacity = 128;

        SortedVector<KeyCombinationBinding, staticCombinationCapacity> keybinds;

        ForceInline KeyCombinationBinding* findKeyCombination(const u64 combination) const noexcept {
            KeyCombinationBinding dummy(combination);
            return this->keybinds.find(dummy);
        }
    public:
        Keymap() = default;
        /**
         * @brief Construct a new Keymap with given
         * initial number of keybinds + the amount specified
         * in `getStaticCombinationCapacity()`.
         * 
         * @param initialNumberOfKeybinds initial number of keybinds
         * allocated *dynamically*
         */
        explicit Keymap(const size_t initialNumberOfKeybinds) : keybinds(initialNumberOfKeybinds) {}
        ~Keymap() = default;
        Keymap(const Keymap& other) = delete;
        Keymap(Keymap&& other) noexcept : keybinds(std::move(other.keybinds)) {}
        Keymap& operator=(const Keymap& other) = delete;
        Keymap& operator=(Keymap&& other) noexcept {
            this->keybinds = std::move(other.keybinds);

            return *this;
        }

        /**
         * @brief Get the keybind capacity that is allocated statically
         * without heap allocation.
         */
        static constexpr u64 getStaticCombinationCapacity() noexcept { return Keymap::staticCombinationCapacity; }

        /**
         * @brief Get the key combination associated with
         * given parameters.
         * 
         * @param numberOfKeys number of keys in the combination, cannot exceed 7
         * @param keyArr a temporary array of KeyboardKey enums
         * 
         * Since the combination is stored as a bitmask,
         * you really shouldn't allocate this on the heap (why bother).
         * This function expects the array to contain
         * at least `numberOfKeys` elements.
         * 
         * The order at which keycodes should be stored is "least recent key first".
         * For example, for Ctrl+Shift+C it'd be {Ctrl, Shift, C}.
         * The actual order stored in the bitmask is, however, reversed
         * and stored as little-endian.
         * @param isPressedArr a temporary array of whether each key
         * in the combination should be pressed
         * 
         * Same expectations as `keyArr`.
         * @param hold whether the combination's callbacks should
         * happen every time input is processed (quite possibly every frame)
         * or should they happen exactly once after the user
         * inputs it
         * @return a bitmask of the key combination or 0 if `numberOfKeys > 7`.
         */
        constexpr u64 getKeyCombinationBitmask(
            const u8 numberOfKeys, const Enums::KeyboardKey* keyArr,
            const bool* isPressedArr, const bool hold
        ) const noexcept;

        /**
         * @brief Register a key combination for this keymap.
         * 
         * @param numberOfKeys number of keys in the combination, cannot exceed 7
         * @param keyArr a temporary array of KeyboardKey enums
         * 
         * Since the combination
         * is stored as a bitmask, you really shouldn't allocate this,
         * especially on the heap. This function expects the array to contain
         * at least `numberOfKeys` elements.
         * @param isPressedArr a temporary array of whether each key
         * in the combination should be pressed
         * 
         * Same expectations as `keyArr`.
         * @param hold whether the combination's callbacks should
         * happen every time input is processed (quite possibly every frame)
         * or should they happen exactly once after the user
         * inputs it
         * @return a bitmask of the key combination, used for easier access
         * to it or 0 if `numberOfKeys > 7` or if internal allocation fails.
         * 
         * To get more information about the combination bitmask,
         * see documentation for `getKeyCombinationBitmask(...)`.
         */
        u64 registerKeyCombination(
            const u8 numberOfKeys, const Enums::KeyboardKey* keyArr,
            const bool* isPressedArr, const bool hold
        ) noexcept;

        /**
         * @brief Register a callback for a given key combination.
         * 
         * @param combination bitmask of the key combination, obtained either
         * from `registerKeyCombination(...)` or `getKeyCombinationBitmask(...)`.
         * @param callback a callback
         * @return whether the callback was successfuly registered.
         * If so, `callback`'s ownership is transferred to this keymap.
         * If not, it stays intact.
         */
        bool registerKeyCombinationCallback(
            const u64 combination, std::function<void()>&& callback
        ) noexcept;

        bool isKeyCombinationHold(const u64 combination) const noexcept;

        /**
         * @brief Get callbacks bound to a given combination.
         * 
         * @param combination key combination bitmask
         * @return pointer to an existing EventNotifier or nullptr if
         * the combination is not registered
         */
        EventNotifier<1>* getKeyCombinationCallbacks(const u64 combination) const noexcept;
};

class Game;

class InputHandler {
    friend class Game;

    private:
        struct EventBuffer latestEvents;
        /**
         * @brief A pointer to a Keymap instance.
         * A pointer, since keymaps should be hot-swappable
         * (for example to have different keybinds for the menu
         * and the world).
         */
        Keymap* currentKeymap = nullptr;
        u64 currentKeyCombination = 0;
        //Masks continously stripping away
        //the least recently pressed/released key.
        static constexpr u64 keyCombinationMasks[] = {
            (u64)-1, //all 1s = most recent 7 keys
            ((u64)0xBF << 56) | (u64)0xFFFFFFFFFFFF, //most recent 6 keys
            ((u64)0x9F << 56) | (u64)0xFFFFFFFFFF, //most recent 5 keys
            ((u64)0x8F << 56) | (u64)0xFFFFFFFF, //most recent 4 keys
            ((u64)0x87 << 56) | (u64)0xFFFFFF, //most recent 3 keys
            ((u64)0x83 << 56) | (u64)0xFFFF, //most recent 2 keys
            ((u64)0x81 << 56) | (u64)0xFF //most recent key
        };
    public:
        const SDL_Event& getLatestEvent() const { 
            return latestEvents.buffer[latestEvents.index];
        }
        
        void processInput(Game& game);

        void checkForKeyCombination(u64 comb);

        /**
         * @brief Swap currently bound keymap with the given one.
         * @param newKeymap new keymap
         * @return previous keymap
         */
        NoDiscard Keymap* swapKeymap(Keymap* newKeymap) {
            Keymap* old = this->currentKeymap;
            this->currentKeymap = newKeymap;
            return old;
        }
};