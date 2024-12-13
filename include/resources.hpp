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

typedef enum {
    //Causes the texture to be loaded immediately
    TextureFlags_LoadImmediately = 1,
    //Copies the path into a dynamically allocated buffer
    TextureFlags_CopyPath = 1 << 1,
    //Whether the texture is tied to a text box
    TextureFlags_Text = 1 << 2,
} TextureFlags;

typedef struct {
    SDL_Texture* texture;
    const char* location;
    u32 flags;
    u32 milisecondsToUnload;
    u64 lastAccessedAt;
} TextureData;

/**
 * @brief Font style enum, mapped
 * to SDL_TTF's macros for convenience.
 */
typedef enum {
    FontStyle_Normal = 0x0,
    FontStyle_Bold = 0x1,
    FontStyle_Italic = 0x2,
    FontStyle_Underline = 0x4,
    FontStyle_Strikethrough = 0x8
} FontStyle;

typedef struct {
    TTF_Font* font;
    const char* location;
    FontStyle style;
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
        TextureHandle registerTexture(const char* path, const u32 flags, const u32 maxTimeLoaded);
        
        /**
         * @brief Get a pointer to the texture with a given handle.
         * It is a handle to internal data, do NOT modify.
         * 
         * @param handle handle to the texture, obtained from
         * `ResourceManager::registerTexture(path)`.
         * @return  pointer to internal texture data
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

        u32 loadFont(const char* path);
        TTF_Font* getFont(u32 id);
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