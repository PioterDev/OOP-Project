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

class RenderableObject;

typedef uint32_t TextureHandle;
typedef uint32_t FontHandle;

/**
 * @brief Flags for textures, loaded
 * from a file or created dynamically.
 */
typedef enum {
    //Causes the texture to be loaded immediately
    TextureFlags_LoadImmediately = 1 << 0,
    //Copies the path into a dynamically allocated buffer
    TextureFlags_CopyPath = 1 << 1,
    //Whether the texture is tied to a text box
    TextureFlags_Text = 1 << 2,
} TextureFlags;

typedef struct {
    SDL_Texture* texture;
    //TODO: change it back to const char*
    char* location;
    u32 flags;
    u32 milisecondsToUnload;
    u64 lastAccessedAt;
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
    FontStyle style;
    FontDirection direction;
    FontWrapAlignment wrapAlignment;
    u32 size;
} FontAttributes;

typedef struct {
    TTF_Font* font;
    const char* location;
    /**
     * @brief Properties of the font.
     * Stores compressed data from `FontStyle`,
     * `FontDirection` and `FontWrapAlignment`
     * in the following way:
     * 
     * Bits 0-3 are used for `FontStyle`,
     * bits 4-5 are used for `FontDirection`,
     * bits 6-7 are used for `FontWrapAlignment`,
     * bits 8-23 are used for the font's size
     * bits 24-31 are currently unused.
     */
    u32 properties;
} FontData;

class ResourceManager {
    friend class Program;

    private:
        vector<TextureData> textures;
        vector<Mix_Chunk*> soundEffects;
        vector<Mix_Music*> music;
        vector<FontData> fonts;
        Status latestStatus = Status::SUCCESS;

        /**
         * @brief Creates a fallback texture.
         * 
         * @return SDL_Texture* to the fallback texture
         * 
         * If this function return a nullptr, the game cannot be
         * safely started as other textures will most likely
         * fail to be created as well.
         */
        SDL_Texture* createFallbackTexture();

        /**
         * @brief Loads the texture of the given handle.
         * 
         * @param handle texture handle
         * @return Status
         */
        Status loadTexture(TextureHandle handle);

        bool isTextureHandleValid(const TextureHandle handle) { return handle < this->textures.size(); }
        bool isFontHandleValid(const FontHandle handle) { return handle < this->fonts.size(); }
        //TODO: add these 2
        // bool isHandleValid(const SoundHandle handle);
        // bool isHandleValid(const MusicHandle handle);
    public:
        Status init();
        void shutdown();

        Status getLatestStatus() const { return this->latestStatus; }

        /**
         * @brief Registers a texture in the resource manager's registry.
         * 
         * @param path path to the file with texture. Be mindful that
         * passing a variable created on the stack WILL most likely
         * CRASH THE PROGRAM if the string is not copied and the texture
         * is not loaded immediately AND set to be always loaded.
         * This function has no way of verifying that the passed
         * pointer is safe to store.
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
        TextureHandle registerTexture(char* path, const u32 flags, const u32 maxTimeLoaded);
        
        /**
         * @brief Creates a renderable text box.
         * 
         * @param text 
         * @param flags texture flags OR'ed together.
         * `TextureFlags_LoadImmediately` has no effect here,
         * `TextureFlags_CopyPath` copies `text` into a separate buffer,
         * `TextureFlags_Text` is always applied, even if not specified
         * @param font handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @param foregroundColor color for the foreground of the text box 
         * @return a TextureHandle, see `ResourceManager::registerTexture(...)`
         * for details.
         */
        TextureHandle createTextTexture(
            char* text, const u32 flags, const FontHandle font,
            const Color foregroundColor, const u32 wrapLength
        );
        
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
        SDL_Texture* getTexture(TextureHandle handle);
        /**
         * @brief Query the texture for its original size.
         * 
         * @param handle handle to the texture
         * @return size of the texture
         */
        Size getTextureOriginalSize(TextureHandle handle);


        u32 loadSoundEffect(const char* path);
        Mix_Chunk* getSoundEffect(u32 id);


        u32 loadMusic(const char* path);
        Mix_Music* getMusic(u32 id);

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
        FontHandle loadFont(const char* path, const u32 size) {
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
        FontHandle loadFont(const char* path, const FontAttributes attributes);
        /**
         * @brief Get the raw pointer to the internal font object
         * with a given font handle.
         * It is a handle to internal data, do NOT modify.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return pointer to internal font object
         */
        TTF_Font* getFont(FontHandle id);

        /**
         * @brief Query the size of the font with a given handle.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return size of the font
         * or 0 if `id` is invalid
         */
        u32 queryFontSize(FontHandle id);
        /**
         * @brief Query the style of the font with a given handle.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return style of the font
         * or FontStyle::Invalid` if `id` is invalid
         */
        FontStyle queryFontStyle(FontHandle id);
        /**
         * @brief Query the direction of the font with a given handle.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return direction of the font
         * or FontDIrection::Invalid` if `id` is invalid
         */
        FontDirection queryFontDirection(FontHandle id);
        /**
         * @brief Query the wrap alignment of the font with a given handle.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return wrap alignment of the font
         * or FontWrapAlignment::Invalid` if `id` is invalid
         */
        FontWrapAlignment queryFontWrapAlignment(FontHandle id);
        /**
         * @brief Query the font's attributes.
         * 
         * @param id handle to the font, obtained from
         * `ResourceManager::loadFont(...)`.
         * @return FontAttributes struct filled with the
         * font's attributes. If `id` is invalid, attributes
         * stored are the same as in queries for individual ones.
         */
        FontAttributes queryFontAttributes(FontHandle id);

};

/**
 * @brief A wrapper around SDL_Mixer's Mix_Chunk.
 * 
 * You can use it for music, the difference between
 * SoundEffect and Music is that SoundEffect is decoded
 * once, while Music is decoded on the fly, meaning this
 * takes up more memory, but less CPU and sound effects
 * are more likely to be invoked frequently than music
 * tracks.
 */
class SoundEffect {
    friend class ResourceManager;

    private:
        Mix_Chunk* soundEffect = nullptr;
        u32 volume = 128;
    public:
        void setVolume(u32 newVolume) {
            Mix_VolumeChunk(this->soundEffect, newVolume);
            this->volume = newVolume <= 128 ? newVolume : 128;
        }
        u32 getVolume() const { return this->volume; }

        NoDiscard Status loadSoundEffect(const char* path);
        NoDiscard Status loadSoundEffect(const string& path) { return loadSoundEffect(path.c_str()); }

        /**
         * @brief Destroys the sound effect data and resets
         * the internal pointer to `NULL`.
         * 
         * This function exists for the same
         * reason that `Texture::destroy()`
         * exists.
         */
        void destroy() {
            Mix_FreeChunk(this->soundEffect);
            this->soundEffect = nullptr;
        }
};

/**
 * @brief A wrapper around SDL_Mixer's Mix_Music.
 * 
 * You can use it for sound effects, but really shouldn't
 * for performance reasons (see the difference in SoundEffect).
 */
class Music {
    friend class ResourceManager;

    private:
        Mix_Music* music = nullptr;
        //Volume for music is global
    protected:
        /**
         * @brief Destroys the music data and resets
         * the internal pointer to `NULL`.
         * 
         * This function exists for the same
         * reason that `Texture::destroy()`
         * exists.
         */
        void destroy() {
            Mix_FreeMusic(this->music);
            this->music = nullptr;
        }
    public:
        NoDiscard Status loadMusic(const char* path);
        NoDiscard Status loadMusic(const string& path) { return loadMusic(path.c_str()); }

};



#endif /* Header */