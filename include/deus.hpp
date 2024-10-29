#pragma once

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
     * 
     */
    enum class Status {
        SUCCESS,
        FAILURE,
        LOGGER_FAILURE,
        SDL_INIT_FAILURE,
        SDL_IMAGE_INIT_FAILURE,
        SDL_MIXER_INIT_FAILURE,
        SDL_MIXER_RESERVE_CHANNELS_FAILURE,
        WINDOW_CREATION_FAILURE,
        RENDERER_CREATION_FAILURE,
        ACCESS_DENIED,
        NONEXISTENT,
        TEXTURE_LOAD_FAILURE,
        TEXTURE_QUERY_FAILURE,
        SOUND_LOAD_FAILURE,
        MUSIC_LOAD_FAILURE,
        FALLBACK_TEXTURE_CREATION_FAILURE
    };

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
}

namespace Structs {
    /**
     * @brief 2D point with integer coordinates.
     * 
     */
    typedef struct {
        int32_t x, y;
    } Point2D_i32;

    /**
     * @brief 2D point with floating point coordinates.
     * 
     */
    typedef struct {
        float x, y;
    } Point2D_f32;

    /**
     * @brief 2D point with 64-bit integer coordinates.
     * 
     */
    typedef struct {
        int64_t x, y;
    } Point2D_i64;

    /**
     * @brief 2D point with double-precision floating point coordinates.
     * 
     */
    typedef struct {
        double x, y;
    } Point2D_f64;

    /* Default Point struct */
    typedef Point2D_i32 Point;
    /* Default floating point Point struct */
    typedef Point2D_f32 PointF;

    /**
     * @brief Rectangular size.
     * 
     */
    typedef struct {
        uint32_t width, height;
    } Size32;

    /**
     * @brief Rectangular size with 64-bit unsigned integers.
     * 
     */
    typedef struct {
        uint64_t width, height;
    } Size64;

    /* Default Size struct */
    typedef Size32 Size;



    /* typedef struct {

    } Vector2D; */
}

#define LOGLEVEL 1