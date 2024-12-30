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