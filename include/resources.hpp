#ifndef RESOURCES_HPP

#define RESOURCES_HPP

#include "Bindings.h"

#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

#include "deus.hpp"

using std::string, std::vector;

using namespace Enums;
using namespace Structs;

extern const char* emptyCString;
extern const char* noErrorCString;

class RenderableObject;

//TODO: finish ResourceManager in terms of SFX, Music
//and documentation

typedef uint32_t TextureHandle;
typedef uint32_t SFXHandle;
typedef uint32_t MusicHandle;
typedef uint32_t FontHandle;

/**
 * @brief Flags for textures, loaded
 * from a file or created dynamically.
 * 
 * Flags 6-31 are reserved for internal use.
 */
typedef enum {
    //Causes the texture to be loaded immediately
    TextureFlags_LoadImmediately = 1 << 0,
    //Copies the path into a dynamically allocated buffer
    TextureFlags_CopyPath = 1 << 1,
    
    //Whether the texture is tied to a UTF-8 text box.
    //Mutually excludes `TextureFlags_Text_UTF16`.
    TextureFlags_Text_UTF8 = 1 << 2,
    //Whether the texture is tied to a UTF-16 text box.
    //Mutually excludes `TextureFlags_Text_UTF8`.
    TextureFlags_Text_UTF16 = 1 << 3,

    //Nearest pixel sampling
    TextureFlags_ScaleModeNearest = 0 << 4,
    //Linear filtering
    TextureFlags_ScaleModeLinear = 1 << 4,
    //Anisotropic filtering
    TextureFlags_ScaleModeBest = 2 << 4,
    //Internal flag to signal whether a handle is valid.
    TextureFlags_IsValid = 1 << 31
} TextureFlags;

typedef struct TextureData {
    SDL_Texture* texture = nullptr;
    const char* location = nullptr;
    u32 flags = 0;
    u32 milisecondsToUnload = 0;
    u64 lastAccessedAt = 0;
} TextureData;

/**
 * @brief Font style enum, mapped
 * to SDL_TTF's macros for convenience.
 */
enum class FontStyle : int {
    Normal = 0x0,
    Bold = 0x1,
    Italic = 0x2,
    Underline = 0x4,
    Strikethrough = 0x8,
    Invalid = 0xF
};

enum class FontDirection : int {
    LR = 0, //Left to right
    RL = 1, //Right to left
    TB = 2, //Top to bottom
    BT = 3, //Bottom to top
    Invalid = 4 //unused in the actual font attributes, but useful for invalid handles
};

enum class FontWrapAlignment : int {
    Left = 0,
    Center = 1,
    Right = 2,
    Invalid = 3
};

typedef struct {
    FontStyle style = FontStyle::Normal;
    FontDirection direction = FontDirection::LR;
    FontWrapAlignment wrapAlignment = FontWrapAlignment::Left;
    u32 size = 0;
} FontAttributes;

typedef struct FontData {
    TTF_Font* font;
    const char* location;
    /**
     * @brief Properties of the font.
     * Stores compressed data from `FontStyle`,
     * `FontDirection` and `FontWrapAlignment`
     * in the following way:
     * 
     * Bits 0-3 are used for `FontStyle`,
     * 
     * bits 4-5 are used for `FontDirection`,
     * 
     * bits 6-7 are used for `FontWrapAlignment`,
     * 
     * bits 8-23 are used for the font's size,
     * 
     * bits 24-31 are currently unused.
     */
    u32 properties;
} FontData;

class ResourceManager {
    friend class Program;

    private:
        enum class TextEncoding {
            UTF8,
            UTF16
        };

        vector<TextureData> textures;
        vector<Mix_Chunk*> soundEffects;
        vector<Mix_Music*> music;
        vector<FontData> fonts;

        Status latestStatus = Status::SUCCESS;
        const char* errorMessage = emptyCString;



        bool isTextureHandleValid(const TextureHandle handle) const noexcept;
        //TODO: add these 2
        // bool isHandleValid(const SoundHandle handle);
        // bool isHandleValid(const MusicHandle handle);
        bool isFontHandleValid(const FontHandle handle) const noexcept;

        /**
         * @brief Creates a fallback texture.
         * 
         * @return SDL_Texture* to the fallback texture
         * 
         * If this function return a nullptr, the game cannot
         * safely start as other textures will most likely
         * fail to be created as well.
         */
        SDL_Texture* __createFallbackTexture() noexcept;

        Status __registerTextureAt(TextureHandle handle, const char* path, const u32 flags) noexcept;

        TextureHandle __createTextTexture(
            const void* text, const u32 flags, const FontHandle font,
            const Color foregroundColor, const u32 wrapLength, const TextEncoding encoding
        ) noexcept;

        Status __createTextTextureAt(
            TextureHandle handle,
            const void* text, const u32 flags, const FontHandle font,
            const Color foregroundColor, const u32 wrapLength, const TextEncoding encoding
        ) noexcept;
    public:
        Status init() noexcept;
        void shutdown() noexcept;

        Status getLatestStatus() const noexcept { return this->latestStatus; }

        const char* getLatestError() const noexcept { return this->errorMessage; }

        /**
         * @brief Register a new texture in the resource manager's registry.
         * 
         * @param path path to the file with texture. Be mindful that
         * passing a variable created on the stack WILL most likely
         * CRASH THE PROGRAM if the string is not copied and the texture
         * is not loaded immediately AND set to be always loaded.
         * This function has no way of verifying that the passed
         * pointer is safe to store.
         * 
         * It is allowed that the file at `path` may not exist
         * when calling this function if `TextureFlags_LoadImmediately`
         * is not specified.
         * @param flags texture flags OR'ed together
         * @param maxTimeLoaded how long should the texture remain loaded
         * in memory after being requested. After this time, the texture is unloaded
         * and memory is freed. Setting it to 0 removes the timer.
         * Again, if the texture is unloaded and the path was a string created
         * on the stack, only God knows what will happen. DO. NOT. DO. THAT!
         * @return a unique index into the texture registry or 0
         * on failure, for further information call
         * `ResourceManager::getLatestStatus()` to get a status code
         * 
         * The texture handle will not change throughout
         * the duration of the program, even when the texture
         * is destroyed. The caller is required to store the returned
         * value, otherwise it won't be able to bind to the texture.
         * 
         * Texture at index 0 is always valid and used as a fallback
         * if loading the texture from `path` fails.
         */
        TextureHandle registerTexture(const char* path, const u32 flags) noexcept;
        
        /**
         * @brief Registers a texture in the resource manager's
         * registry using an existing handle.
         * 
         * @param handle handle to the texture
         * @param path path to the file with a texture,
         * same restrictions apply as with
         * `ResourceManager::registerTexture(...)`.
         * @param flags texture flags OR'ed together
         * @return Status 
         */
        Status registerTextureAt(TextureHandle handle, const char* path, const u32 flags) noexcept {
            return this->__registerTextureAt(handle, path, flags);
        }
        
        /**
         * @brief Creates a renderable text box using a UTF-8 character set.
         * 
         * @param text text to make into a texture
         * @param flags texture flags OR'ed together.
         * `TextureFlags_LoadImmediately` has no effect here,
         * `TextureFlags_CopyPath` copies `text` into a separate buffer,
         * `TextureFlags_Text_...` is always applied, even if not specified
         * @param font handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @param foregroundColor color for the foreground of the text box 
         * @return a TextureHandle, see `ResourceManager::registerTexture(...)`
         * for details.
         */
        ForceInline TextureHandle createTextTexture(
            const char* text, const u32 flags, const FontHandle font,
            const Color foregroundColor, const u32 wrapLength
        ) noexcept {
            return this->__createTextTexture(
                text, flags, font, foregroundColor, wrapLength, TextEncoding::UTF8
            );
        }

        /**
         * @brief Creates a renderable text box using a UTF-16 character set.
         * 
         * @param text text to make into a texture
         * @param flags texture flags OR'ed together.
         * `TextureFlags_LoadImmediately` has no effect here,
         * `TextureFlags_CopyPath` copies `text` into a separate buffer,
         * `TextureFlags_Text_...` is always applied, even if not specified
         * @param font handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @param foregroundColor color for the foreground of the text box 
         * @return a TextureHandle, see `ResourceManager::registerTexture(...)`
         * for details.
         */
        ForceInline TextureHandle createTextTexture(
            const char16_t* text, const u32 flags, const FontHandle font,
            const Color foregroundColor, const u32 wrapLength
        ) noexcept {
            return this->__createTextTexture(
                text, flags, font, foregroundColor, wrapLength, TextEncoding::UTF16
            );
        }

        /**
         * @brief Creates a renderable text box using a UTF-8 character set
         * in an existing handle.
         * 
         * @param handle handle to the texture
         * @param text text to make into a texture
         * @param flags texture flags OR'ed together.
         * `TextureFlags_LoadImmediately` has no effect here,
         * `TextureFlags_CopyPath` copies `text` into a separate buffer,
         * `TextureFlags_Text_...` is always applied, even if not specified
         * @param font handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @param foregroundColor color for the foreground of the text box 
         * @return TODO: status
         */
        ForceInline Status createTextTextureAt(
            TextureHandle handle,
            const char* text, const u32 flags, const FontHandle font,
            const Color foregroundColor, const u32 wrapLength
        ) noexcept {
            return this->__createTextTextureAt(
                handle, text, flags, font, foregroundColor, wrapLength, TextEncoding::UTF8
            );
        }

        /**
         * @brief Creates a renderable text box using a UTF-16 character set
         * in an existing handle.
         * 
         * @param handle handle to the texture
         * @param text text to make into a texture
         * @param flags texture flags OR'ed together.
         * `TextureFlags_LoadImmediately` has no effect here,
         * `TextureFlags_CopyPath` copies `text` into a separate buffer,
         * `TextureFlags_Text_...` is always applied, even if not specified
         * @param font handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @param foregroundColor color for the foreground of the text box 
         * @return TODO: status
         */
        ForceInline Status createTextTextureAt(
            TextureHandle handle,
            const char16_t* text, const u32 flags, const FontHandle font,
            const Color foregroundColor, const u32 wrapLength
        ) noexcept {
            return this->__createTextTextureAt(
                handle, text, flags, font, foregroundColor, wrapLength, TextEncoding::UTF16
            );
        }
        
        /**
         * @brief Get a pointer to the texture with a given texture handle.
         * It is a handle to internal data, do NOT modify.
         * 
         * @param handle handle to the texture, obtained from
         * `ResourceManager::registerTexture(path)`.
         * @return pointer to internal texture object
         * 
         * If you need to modify the texture, do NOT do it yourself,
         * it is dangerous, risks memory leaks and potentially crashes.
         * Instead use the ResourceManager, as it has a broader context.
         */
        SDL_Texture* getTexture(TextureHandle handle) noexcept;
        /**
         * @brief Query the texture for its original size.
         * 
         * @param handle handle to the texture
         * @return size of the texture
         */
        Size getTextureOriginalSize(TextureHandle handle) noexcept;

        /**
         * @brief Reserves a texture handle for later use.
         * 
         * @return a unique index into the texture registry or 0
         * on failure, call `getLatestError()` for more information.
         * 
         * The texture handle will not change throughout
         * the duration of the program, even when the texture
         * is destroyed. The caller is required to store the returned
         * value, otherwise it won't be able to bind to the texture.
         */
        TextureHandle reserveTextureHandle() noexcept;

        /**
         * @brief Loads the texture of the given handle.
         * 
         * @param handle handle to the texture
         * @return `Status::SUCCESS` on success,
         * a variety of status codes on failure;
         * call `getLatestError()` for more information.
         */
        Status loadTexture(TextureHandle handle) noexcept;

        /**
         * @brief Unloads the texture of the given handle.
         * 
         * @param handle handle to the texture
         * @return `Status::SUCCESS` on success,
         * 
         * `Status::INVALID_ARGS` if `handle` is invalid.
         */
        Status unloadTexture(TextureHandle handle) noexcept;

        /**
         * @brief Destroys a given texture, freeing
         * its handle for other textures and invalidating it.
         * 
         * @param handle handle to the texture
         * 
         * @return Status
         */
        Status destroyTexture(TextureHandle handle) noexcept;

        /**
         * @brief Whether the texture at the given
         * handle is a text texture.
         * 
         * @param handle handle to the texture
         */
        bool isTextTexture(TextureHandle handle) noexcept;


        /**
         * @brief Loads a sound effect from a given path at
         * default volume (12.5% of maximum).
         * 
         * You can use it for music, the difference between
         * SoundEffect and Music is that SoundEffect is decoded
         * once, while Music is decoded on the fly, meaning this
         * takes up more memory, but less CPU and sound effects
         * are more likely to be invoked frequently than music
         * tracks.
         */
        u32 loadSoundEffect(const char* path) noexcept {
            return this->loadSoundEffect(path, 16);
        }
        /**
         * @brief Loads a sound effect from a given path.
         * 
         * You can use it for music, the difference between
         * SoundEffect and Music is that SoundEffect is decoded
         * once, while Music is decoded on the fly, meaning this
         * takes up more memory, but less CPU and sound effects
         * are more likely to be invoked frequently than music
         * tracks.
         * 
         * @param path path to the audio file
         * @param volume initial volume of the sound effect
         * 
         * @return a unique index into the (TODO: this shit)
         */
        SFXHandle loadSoundEffect(const char* path, u8 volume) noexcept;

        Mix_Chunk* getSoundEffect(u32 id) noexcept;


        MusicHandle loadMusic(const char* path) noexcept;
        Mix_Music* getMusic(u32 id) noexcept;

        /**
         * @brief Loads a font dynamically from a given path with default
         * style (normal), direction (left to right) and wrap alignment
         * (left).
         * 
         * @param path path to the file with font data
         * @param size initial glyph size of the font
         * @return a unique index into the font registry or 0
         * on failure, for further information call
         * `ResourceManager::getLatestStatus()` to get a status code
         * 
         * Same rules that apply to texture handles apply here,
         * see `ResourceManager::registerTexture(...)` for details.
         */
        FontHandle loadFont(const char* path, const u32 size) noexcept {
            return this->loadFont(
                path, {
                    FontStyle::Normal,
                    FontDirection::LR,
                    FontWrapAlignment::Left,
                    size
                }
            );
        }
        /**
         * @brief Loads a font dynamically from a given path.
         * 
         * @param path path to the file with font data
         * @param attributes attributes of the font, see `FontAttributes` struct definition
         * @return a unique index into the font registry or 0
         * on failure, for further information call
         * `ResourceManager::getLatestStatus()` to get a status code
         * 
         * Same rules that apply to texture handles apply here,
         * see `ResourceManager::registerTexture(...)` for details.
         */
        FontHandle loadFont(const char* path, const FontAttributes attributes) noexcept;
        /**
         * @brief Get the raw pointer to the internal font object
         * with a given font handle.
         * It is a handle to internal data, do NOT modify.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return pointer to internal font object or nullptr on error
         */
        TTF_Font* getFont(FontHandle id) noexcept;

        /**
         * @brief Query the size of the font with a given handle.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return size of the font
         * or 0 if `id` is invalid
         */
        u32 queryFontSize(FontHandle id) noexcept;
        /**
         * @brief Query the style of the font with a given handle.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return style of the font
         * or FontStyle::Invalid` if `id` is invalid
         */
        FontStyle queryFontStyle(FontHandle id) noexcept;
        /**
         * @brief Query the direction of the font with a given handle.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return direction of the font
         * or FontDIrection::Invalid` if `id` is invalid
         */
        FontDirection queryFontDirection(FontHandle id) noexcept;
        /**
         * @brief Query the wrap alignment of the font with a given handle.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return wrap alignment of the font
         * or FontWrapAlignment::Invalid` if `id` is invalid
         */
        FontWrapAlignment queryFontWrapAlignment(FontHandle id) noexcept;
        /**
         * @brief Query the font's attributes.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return FontAttributes struct filled with the
         * font's attributes. If `id` is invalid, attributes
         * stored are the same as in queries for individual ones.
         */
        FontAttributes queryFontAttributes(FontHandle id) noexcept;

};

/**
 * @brief A wrapper around SDL_Mixer's Mix_Music.
 * 
 * You can use it for sound effects, but really shouldn't
 * for performance reasons (see the difference in SoundEffect).
 */



#endif /* Header */