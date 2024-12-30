#pragma once

/**
 * @brief A header file containing
 * #defines, enums, unions and structs, hence "deus".
 * It also contains useful typedefs.
 */

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;

namespace Enums {
    /**
     * @brief Enum for returning
     * the status of an operation.
     * Used throughout the entire project.
     */
    enum class Status : u32 {
        SUCCESS,
        FAILURE,
        INVALID_ARGS,
        NULL_PASSED,
        ALLOC_FAILURE,

        ACCESS_DENIED,
        NONEXISTENT,
        ALREADY_EXISTS,
        OUT_OF_BOUNDS,

        LOGGER_FAILURE,

        SDL_FAILURE = 0x400, //Generic SDL failure
        SDL_INIT_FAILURE, //Initialization of SDL failed
        SDL_WINDOW_CREATION_FAILURE,
        SDL_RENDERER_CREATION_FAILURE,

        SDL_SURFACE_CREATION_FAILURE,
        SDL_TEXTURE_CREATION_FAILURE,
        SDL_TEXTURE_QUERY_FAILURE,
        
        SOUND_LOAD_FAILURE,
        MUSIC_LOAD_FAILURE,
        
        FALLBACK_TEXTURE_CREATION_FAILURE,
        
        FONT_LOAD_FAILURE,

        SDL_IMAGE_FAILURE = 0x740, //Generic SDL_image failure
        SDL_IMAGE_INIT_FAILURE, //Initialization of SDL_image failed
        SDL_IMAGE_LOADTEXTURE_FAILURE, //

        SDL_MIXER_FAILURE = 0x780, //Generic SDL_mixer failure
        SDL_MIXER_INIT_FAILURE, //Initialization of SDL_mixer failed
        SDL_MIXER_RESERVE_CHANNELS_FAILURE,

        SDL_TTF_FAILURE = 0x7C0, //Generic SDL_ttf failure
        SDL_TTF_INIT_FAILURE, //Initialization of SDL_ttf failed
        SDL_TTF_RENDERTEXT_FAILURE,

        SDL_LAST = 0x7FF, //A sentinel value to mark the last SDL-related status code
    };

    /**
     * @brief Whether given status is related to SDL.
     * @param s Status enum
     */
    inline bool is_status_SDL(const Status s) {
        return 
            (static_cast<int>(s) >= static_cast<int>(Status::SDL_FAILURE)) &&
            (static_cast<int>(s) <= static_cast<int>(Status::SDL_LAST));
    }
    /**
     * @brief Whether given status is related to SDL_image.
     * @param s Status enum
     */
    inline bool is_status_SDL_image(const Status s) {
        return 
            (static_cast<int>(s) >= static_cast<int>(Status::SDL_IMAGE_FAILURE)) &&
            (static_cast<int>(s) <= static_cast<int>(Status::SDL_LAST));  
    }
    /**
     * @brief Whether given status is related to SDL_mixer.
     * @param s Status enum
     */
    inline bool is_status_SDL_mixer(const Status s) {
        return 
            (static_cast<int>(s) >= static_cast<int>(Status::SDL_MIXER_FAILURE)) &&
            (static_cast<int>(s) <= static_cast<int>(Status::SDL_LAST));   
    }
    /**
     * @brief Whether given status is related to SDL_ttf.
     * @param s Status enum
     */
    inline bool is_status_SDL_ttf(const Status s) {
        return 
            (static_cast<int>(s) >= static_cast<int>(Status::SDL_TTF_FAILURE)) &&
            (static_cast<int>(s) <= static_cast<int>(Status::SDL_LAST));   
    }

    typedef enum {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    } LogLevel;

    typedef enum {
        TEXTURE,
        SFX,
        MUSIC,
        FONT,
        RESOURCETYPE_AMOUNT
    } ResourceType;

    enum class ThreadStatus {
        CONTINUE, //free to continue execution
        STOP,     //loop should stop and execution ended
        PAUSE,    //loop should be skipped and execution suspended
    };

    /* enum class MathFunctionType {
        POLYNOMIAL,
        EXPONENTIAL,
        LOGARITHMIC,
        TRIGONOMETRIC,
        HYPERBOLIC,

    }; */

    enum class ShapeType {
        SQUARE,
        RECTANGLE,
        CIRCLE,
        ELLIPSE
    };

    /**
     * @brief An enum remapping SDL_scancode enum
     * into a contigous one.
     */
    typedef enum {
        KeyboardKey_UNKNOWN,
        KeyboardKey_A,
        KeyboardKey_B,
        KeyboardKey_C,
        KeyboardKey_D,
        KeyboardKey_E,
        KeyboardKey_F,
        KeyboardKey_G,
        KeyboardKey_H,
        KeyboardKey_I,
        KeyboardKey_J,
        KeyboardKey_K,
        KeyboardKey_L,
        KeyboardKey_M,
        KeyboardKey_N,
        KeyboardKey_O,
        KeyboardKey_P,
        KeyboardKey_Q,
        KeyboardKey_R,
        KeyboardKey_S,
        KeyboardKey_T,
        KeyboardKey_U,
        KeyboardKey_V,
        KeyboardKey_W,
        KeyboardKey_X,
        KeyboardKey_Y,
        KeyboardKey_Z,
        KeyboardKey_1,
        KeyboardKey_2,
        KeyboardKey_3,
        KeyboardKey_4,
        KeyboardKey_5,
        KeyboardKey_6,
        KeyboardKey_7,
        KeyboardKey_8,
        KeyboardKey_9,
        KeyboardKey_0,
        KeyboardKey_RETURN,
        KeyboardKey_ESCAPE,
        KeyboardKey_BACKSPACE,
        KeyboardKey_TAB,
        KeyboardKey_SPACE,
        KeyboardKey_MINUS,
        KeyboardKey_EQUALS,
        KeyboardKey_LEFTBRACKET,
        KeyboardKey_RIGHTBRACKET,
        KeyboardKey_BACKSLASH,
        KeyboardKey_NONUSHASH,
        KeyboardKey_SEMICOLON,
        KeyboardKey_APOSTROPHE,
        KeyboardKey_GRAVE,
        KeyboardKey_COMMA,
        KeyboardKey_PERIOD,
        KeyboardKey_SLASH,
        KeyboardKey_CAPSLOCK,
        KeyboardKey_F1,
        KeyboardKey_F2,
        KeyboardKey_F3,
        KeyboardKey_F4,
        KeyboardKey_F5,
        KeyboardKey_F6,
        KeyboardKey_F7,
        KeyboardKey_F8,
        KeyboardKey_F9,
        KeyboardKey_F10,
        KeyboardKey_F11,
        KeyboardKey_F12,
        KeyboardKey_PRINTSCREEN,
        KeyboardKey_SCROLLLOCK,
        KeyboardKey_PAUSE,
        KeyboardKey_INSERT,
        KeyboardKey_HOME,
        KeyboardKey_PAGEUP,
        KeyboardKey_DELETE,
        KeyboardKey_END,
        KeyboardKey_PAGEDOWN,
        KeyboardKey_RIGHT,
        KeyboardKey_LEFT,
        KeyboardKey_DOWN,
        KeyboardKey_UP,
        KeyboardKey_NUMLOCKCLEAR,
        KeyboardKey_KP_DIVIDE,
        KeyboardKey_KP_MULTIPLY,
        KeyboardKey_KP_MINUS,
        KeyboardKey_KP_PLUS,
        KeyboardKey_KP_ENTER,
        KeyboardKey_KP_1,
        KeyboardKey_KP_2,
        KeyboardKey_KP_3,
        KeyboardKey_KP_4,
        KeyboardKey_KP_5,
        KeyboardKey_KP_6,
        KeyboardKey_KP_7,
        KeyboardKey_KP_8,
        KeyboardKey_KP_9,
        KeyboardKey_KP_0,
        KeyboardKey_KP_PERIOD,
        KeyboardKey_NONUSBACKSLASH,
        KeyboardKey_APPLICATION,
        KeyboardKey_POWER,
        KeyboardKey_KP_EQUALS,
        KeyboardKey_F13,
        KeyboardKey_F14,
        KeyboardKey_F15,
        KeyboardKey_F16,
        KeyboardKey_F17,
        KeyboardKey_F18,
        KeyboardKey_F19,
        KeyboardKey_F20,
        KeyboardKey_F21,
        KeyboardKey_F22,
        KeyboardKey_F23,
        KeyboardKey_F24,
        KeyboardKey_EXECUTE,
        KeyboardKey_HELP,
        KeyboardKey_MENU,
        KeyboardKey_SELECT,
        KeyboardKey_STOP,
        KeyboardKey_AGAIN,
        KeyboardKey_UNDO,
        KeyboardKey_CUT,
        KeyboardKey_COPY,
        KeyboardKey_PASTE,
        KeyboardKey_FIND,
        KeyboardKey_MUTE,
        KeyboardKey_VOLUMEUP,
        KeyboardKey_VOLUMEDOWN,
        KeyboardKey_KP_COMMA,
        KeyboardKey_KP_EQUALSAS400,
        KeyboardKey_INTERNATIONAL1,
        KeyboardKey_INTERNATIONAL2,
        KeyboardKey_INTERNATIONAL3,
        KeyboardKey_INTERNATIONAL4,
        KeyboardKey_INTERNATIONAL5,
        KeyboardKey_INTERNATIONAL6,
        KeyboardKey_INTERNATIONAL7,
        KeyboardKey_INTERNATIONAL8,
        KeyboardKey_INTERNATIONAL9,
        KeyboardKey_LANG1,
        KeyboardKey_LANG2,
        KeyboardKey_LANG3,
        KeyboardKey_LANG4,
        KeyboardKey_LANG5,
        KeyboardKey_LANG6,
        KeyboardKey_LANG7,
        KeyboardKey_LANG8,
        KeyboardKey_LANG9,
        KeyboardKey_ALTERASE,
        KeyboardKey_SYSREQ,
        KeyboardKey_CANCEL,
        KeyboardKey_CLEAR,
        KeyboardKey_PRIOR,
        KeyboardKey_RETURN2,
        KeyboardKey_SEPARATOR,
        KeyboardKey_OUT,
        KeyboardKey_OPER,
        KeyboardKey_CLEARAGAIN,
        KeyboardKey_CRSEL,
        KeyboardKey_EXSEL,
        KeyboardKey_KP_00,
        KeyboardKey_KP_000,
        KeyboardKey_THOUSANDSSEPARATOR,
        KeyboardKey_DECIMALSEPARATOR,
        KeyboardKey_CURRENCYUNIT,
        KeyboardKey_CURRENCYSUBUNIT,
        KeyboardKey_KP_LEFTPAREN,
        KeyboardKey_KP_RIGHTPAREN,
        KeyboardKey_KP_LEFTBRACE,
        KeyboardKey_KP_RIGHTBRACE,
        KeyboardKey_KP_TAB,
        KeyboardKey_KP_BACKSPACE,
        KeyboardKey_KP_A,
        KeyboardKey_KP_B,
        KeyboardKey_KP_C,
        KeyboardKey_KP_D,
        KeyboardKey_KP_E,
        KeyboardKey_KP_F,
        KeyboardKey_KP_XOR,
        KeyboardKey_KP_POWER,
        KeyboardKey_KP_PERCENT,
        KeyboardKey_KP_LESS,
        KeyboardKey_KP_GREATER,
        KeyboardKey_KP_AMPERSAND,
        KeyboardKey_KP_DBLAMPERSAND,
        KeyboardKey_KP_VERTICALBAR,
        KeyboardKey_KP_DBLVERTICALBAR,
        KeyboardKey_KP_COLON,
        KeyboardKey_KP_HASH,
        KeyboardKey_KP_SPACE,
        KeyboardKey_KP_AT,
        KeyboardKey_KP_EXCLAM,
        KeyboardKey_KP_MEMSTORE,
        KeyboardKey_KP_MEMRECALL,
        KeyboardKey_KP_MEMCLEAR,
        KeyboardKey_KP_MEMADD,
        KeyboardKey_KP_MEMSUBTRACT,
        KeyboardKey_KP_MEMMULTIPLY,
        KeyboardKey_KP_MEMDIVIDE,
        KeyboardKey_KP_PLUSMINUS,
        KeyboardKey_KP_CLEAR,
        KeyboardKey_KP_CLEARENTRY,
        KeyboardKey_KP_BINARY,
        KeyboardKey_KP_OCTAL,
        KeyboardKey_KP_DECIMAL,
        KeyboardKey_KP_HEXADECIMAL,
        KeyboardKey_LCTRL,
        KeyboardKey_LSHIFT,
        KeyboardKey_LALT,
        KeyboardKey_LGUI,
        KeyboardKey_RCTRL,
        KeyboardKey_RSHIFT,
        KeyboardKey_RALT,
        KeyboardKey_RGUI,
        KeyboardKey_MODE,
        KeyboardKey_AUDIONEXT,
        KeyboardKey_AUDIOPREV,
        KeyboardKey_AUDIOSTOP,
        KeyboardKey_AUDIOPLAY,
        KeyboardKey_AUDIOMUTE,
        KeyboardKey_MEDIASELECT,
        KeyboardKey_WWW,
        KeyboardKey_MAIL,
        KeyboardKey_CALCULATOR,
        KeyboardKey_COMPUTER,
        KeyboardKey_AC_SEARCH,
        KeyboardKey_AC_HOME,
        KeyboardKey_AC_BACK,
        KeyboardKey_AC_FORWARD,
        KeyboardKey_AC_STOP,
        KeyboardKey_AC_REFRESH,
        KeyboardKey_AC_BOOKMARKS,
        KeyboardKey_BRIGHTNESSDOWN,
        KeyboardKey_BRIGHTNESSUP,
        KeyboardKey_DISPLAYSWITCH,
        KeyboardKey_KBDILLUMTOGGLE,
        KeyboardKey_KBDILLUMDOWN,
        KeyboardKey_KBDILLUMUP,
        KeyboardKey_EJECT,
        KeyboardKey_SLEEP,
        KeyboardKey_APP1,
        KeyboardKey_APP2,
        KeyboardKey_AUDIOREWIND,
        KeyboardKey_AUDIOFASTFORWARD,
        KeyboardKey_SOFTLEFT,
        KeyboardKey_SOFTRIGHT,
        KeyboardKey_CALL,
        KeyboardKey_ENDCALL,
        KeyboardKey_AMOUNT //not a key, marks the amount of keys supported
    } KeyboardKey;
}

namespace Structs {
    /**
     * @brief 2D point with integer coordinates.
     */
    typedef struct Vector2i32 {
        i32 x, y;
        bool operator==(const Vector2i32& other) const {
            return x == other.x && y == other.y;
        }
    } Vector2i32;

    /**
     * @brief 2D point with floating point coordinates.
     * 
     */
    typedef struct Vector2f {
        float x, y;
        bool operator==(const Vector2f& other) { return x == other.x && y == other.y; }
    } Vector2f;

    /**
     * @brief 2D point with 64-bit integer coordinates.
     * 
     */
    typedef struct Vector2i64 {
        i64 x, y;
        bool operator==(const Vector2i64& other) { return x == other.x && y == other.y; }
    } Vector2i64;

    /**
     * @brief 2D point with double-precision floating point coordinates.
     * 
     * Do not confuse with a 2D vector.
     * 
     * This name is misleading, but uhm...yeah.
     */
    typedef struct Vector2d {
        double x, y;
        bool operator==(const Vector2d& other) { return x == other.x && y == other.y; }
    } Vector2d;

    /* Default Point struct */
    typedef Vector2i32 Point;
    typedef Vector2i32 BlockPos;
    typedef Vector2i32 ChunkPos;
    /* Default floating point Point struct */
    typedef Vector2f PointF;


    /**
     * @brief Struct for counting stuff in 2 dimensions, 16-bit uints.
     */
    typedef struct {
        u16 x, y;
    } Count16;

    /**
     * @brief Struct for counting stuff in 2 dimensions, 32-bit uints.
     */
    typedef struct {
        u32 x, y;
    } Count32;

    /**
     * @brief Struct for counting stuff in 2 dimensions, 64-bit uints.
     */
    typedef struct {
        u64 x, y;
    } Count64;

    /**
     * @brief Rectangular size, 32-bit uints.
     */
    typedef struct {
        u32 width, height;
    } Size32;

    /**
     * @brief Rectangular size, 64-bit bit uints.
     */
    typedef struct {
        u64 width, height;
    } Size64;

    /**
     * @brief Rectangular size, floats.
     */
    typedef struct {
        float width, height;
    } SizeF32;

    /**
     * @brief Rectangular size, doubles.
     */
    typedef struct {
        double width, height;
    } SizeF64;

    /* Default Size struct */
    typedef Size32 Size;

    typedef struct {
        Enums::ShapeType shape;
        PointF center;
        union {
            float diagonal; //square
            struct {
                float x, y;
            } diagonals; //rectangle
            float radius; //circle
            struct {
                float x, y;
            } radii; //ellipse
        };
    } Shape;

    typedef struct {
        float x1, y1;
        float x2, y2;
    } AABB;

    typedef struct {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
    } Color;

    namespace Colors {
        constexpr Color NONE      = {0, 0, 0, 0};

        constexpr Color WHITE     = {255, 255, 255, 255};
        constexpr Color BLACK     = {0, 0, 0, 255};
        constexpr Color GRAY      = {128, 128, 128, 255};
        constexpr Color LIGHTGRAY = {192, 192, 192, 255};
        constexpr Color DARKGRAY  = {64, 64, 64, 255};

        constexpr Color RED       = {255, 0, 0, 255};
        constexpr Color GREEN     = {0, 255, 0, 255};
        constexpr Color BLUE      = {0, 0, 255, 255};
        
        constexpr Color YELLOW    = {255, 255, 0, 255};
        constexpr Color MAGENTA   = {255, 0, 255, 255};
        constexpr Color CYAN      = {0, 255, 255, 255};

        constexpr Color ORANGE    = {255, 128, 0, 255};
    };
}

namespace Unions {
    typedef union {
        u64 uuid;
        char bytes[sizeof(u64)];
        /**
         * @brief We're both assuming little-endian system
         * AND using what's considered undefined
         * behavior by C++ - fabulous!
         */
        struct {
            //Instance number of the object
            u32 instance;
            //A uniquely identifiable object type
            u32 type;
        } parts;
    } UUID;
};

#define LOGLEVEL 0