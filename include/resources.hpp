#ifndef RESOURCES_HPP

#define RESOURCES_HPP

#include "Bindings.h"

#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>

#include "deus.hpp"

using std::string, std::vector;

using namespace Enums;
using namespace Structs;

typedef struct {
    SDL_Texture* texture = nullptr;
    Size originalSize = {0, 0};
} TextureHandle;

class ResourceManager {
    friend class Program;

    private:
        vector<TextureHandle> textures;
        vector<Mix_Chunk*> soundEffects;
        vector<Mix_Music*> music;
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
    public:
        ~ResourceManager();

        Status init();

        Status getLatestStatus() const { return this->latestStatus; }

        /**
         * @brief Registers a texture and loads it from given path.
         * 
         * @param path path to the file with texture
         * @return a unique index into the texture registry or 0
         * on failure, for further information call
         * `ResourceManager::getLatestStatus()` to get a status code
         * 
         * The index of the texture will not change throughout
         * the duration of the game, even when the texture
         * is reloaded. The caller is required to store the returned
         * value, otherwise it won't be able to bind to the texture.
         * 
         * Texture at index 0 is always valid and used as a fallback
         * if loading the texture from `path` fails.
         */
        u32 registerTexture(const char* path);
        
        /**
         * @brief Get a handle to the texture with the given ID.
         * 
         * @param id ID of the texture handle, obtained from
         * `ResourceManager::registerTexture(path)`.
         * @return texture handle 
         * 
         * If you need to modify the texture, do NOT do it yourself,
         * it is dangerous, risks memory leaks and potentially crashes.
         * Instead use the ResourceManager, as it has a broader context.
         */
        TextureHandle getTextureHandle(u32 id) const;

        SDL_Texture* getTexture(u32 id);
        
        Size getTextureOriginalSize(u32 id) const;
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