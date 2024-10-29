#ifndef RESOURCES_HPP

#define RESOURCES_HPP

#include "Bindings.h"

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>

#include "deus.hpp"

using std::string, std::vector;

using namespace Enums;
using namespace Structs;

typedef struct {
    SDL_Texture* texture;
    Size originalSize;
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
        Status init();
        
        ~ResourceManager() {
            for(size_t i = 0; i < textures.size(); i++) {
                SDL_DestroyTexture(textures[i].texture);
            }
            for(size_t i = 1; i < soundEffects.size(); i++) {
                Mix_FreeChunk(soundEffects[i]);
            }
            for(size_t i = 1; i < music.size(); i++) {
                Mix_FreeMusic(music[i]);
            }
        }

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
        uint32_t registerTexture(const char* path);
        
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
        TextureHandle getTextureHandle(uint32_t id) const {
            if(id > this->textures.size()) return textures[0]; //fallback texture
            return textures[id];
        }
        Size getTextureOriginalSize(uint32_t id) const {
            if(id > this->textures.size()) return textures[0].originalSize; //fallback texture
            return textures[id].originalSize;
        }
};

/**
 * @brief A wrapper around SDL_Texture.
 * Provides a basic interface for
 * renderable objects to get the
 * texture for rendering.
 * 
 * The texture is managed internally
 * and externally by the program's resource manager,
 * modifying the texture yourself
 * leads to undefined behavior.
 * 
 * Objects accessing `rect`, `targetPortion`,
 * `angle` and `flip` members
 * of the texture are free to change them however
 * they want though.
 */
class Texture {
    friend class ResourceManager;
    friend class GameRenderer;
    friend class Game;

    private:
        TextureHandle textureHandle;
    protected:
        /**
         * @brief Loads the texture from a given filepath.
         * 
         * @param path path to the texture file
         * @return `Status::SUCCESS` on success
         * 
         * `Status::TEXTURE_LOAD_FAILURE` if texture loading failed
         * 
         * `Status::TEXTURE_QUERY_FAILURE` if obtaining internal
         * information about the texture failed - in this case,
         * the texture was loaded successfully and will be automatically
         * destroyed before returning
         * 
         * `Status::ACCESS_DENIED` if file associated with the texture
         * cannot be opened
         * 
         * `Status::NONEXISTENT` if file doesn't exist
         * 
         * `Status::FAILURE` on any other error
         */
        // NoDiscard Status loadTexture(const char* path);
        // NoDiscard Status loadTexture(const string& path) { return loadTexture(path.c_str()); }
    public:
        /**
         * @brief Rectangle used to select
         * which portion of the texture
         * should be used in rendering.
         */
        SDL_Rect rect = {0, 0, 0, 0};
        /**
         * @brief Rectangle used to select
         * which portion of the rendering target will be filled
         * with the texture.
         */
        SDL_Rect targetPortion = {0, 0, 0, 0};
        /**
         * @brief Angle at which the texture should be rotated
         * when rendering.
         */
        double angle = 0.0;
        /**
         * @brief What flip will be applied when rendering.
         * 
         * Unfortunately it's not possible to flip
         * the texture both horizontally and vertically
         * with this option. To do so, you need to
         * set the flip to none and rotate the texture 180 degrees.
         */
        SDL_RendererFlip flip = SDL_FLIP_NONE;

        /**
         * @brief Binds this texture to externally managed
         * texture handle.
         * 
         * @param handle texture handle, obtained from the
         * ResourceManager with a texture ID
         */
        void bindTexture(TextureHandle handle) { this->textureHandle = handle; }

        SDL_Texture* getTexture() { return this->textureHandle.texture; }

        /**
         * @brief Sets the texture's portion to use.
         * 
         * A texture portion is part of the texture to use in rendering,
         * as opposed to a target portion, which positions the texture on
         * the screen.
         * 
         * @param r rectangle portion of the texture
         */
        void setTexturePortion(SDL_Rect r) { this->rect = r; }

        /**
         * @brief Sets the texture's portion to use
         * to the whole texture.
         */
        void setTexturePortionOriginal() {
            this->rect.x = this->rect.y = 0;
            this->rect.h = (int)this->textureHandle.originalSize.height;
            this->rect.w = (int)this->textureHandle.originalSize.width;
        }

        SDL_Rect* getTexturePortion() { return &this->rect; }

        /**
         * @brief Sets the target portion.
         * 
         * A target portion is a part of the screen to fill
         * with the texture, as opposed to a texture portion,
         * which selects relevant part of the source texture to copy.
         * 
         * @param r rectangle portion of the screen
         */
        void setTargetPortion(SDL_Rect r) { this->targetPortion = r; }

        SDL_Rect* getTargetPortion() { return &this->targetPortion; }

        void setSize(Size newSize) {
            //These casts are undefined behavior if
            //any field >= 2^31, but come on, no texture is that big.

            this->rect.w = (int)newSize.width;
            this->rect.h = (int)newSize.height;
        }

        /**
         * @brief Resets texture size to its original size.
         */
        void resetTextureSize() {
            this->targetPortion.w = (int)this->textureHandle.originalSize.width;
            this->targetPortion.h = (int)this->textureHandle.originalSize.height;
        }

        /**
         * @brief Scales the texture in the X axis (horizontally).
        
         * You can safely ignore the following information 
         * if you won't have to change the texture throughout
         * the lifetime of the object.
         * 
         * Scaling is applied immediately and is not retained throughout
         * different texture handles (i.e. the texture's size
         * will not change if for whatever reason
         * the texture handle is changed and its original size changes).
         * @param scale new horizontal scale
         */
        void scaleX(float scale) {
            this->targetPortion.w = (int)(scale * (float)this->textureHandle.originalSize.width);
        }
        /**
         * @brief Scales the texture in the Y axis (vertically).
         * 
         * You can safely ignore the following information 
         * if you won't have to change the texture throughout
         * the lifetime of the object.
         * 
         * Scaling is applied immediately and is not retained throughout
         * different texture handles (i.e. the texture's size
         * will not change if for whatever reason
         * the texture handle is changed and its original size changes).
         * 
         * @param scale new vertical scale
         */
        void scaleY(float scale) {
            this->targetPortion.h = (int)(scale * (float)this->textureHandle.originalSize.height);
        }
        /**
         * @brief Scales the texture.
         * 
         * You can safely ignore the following information 
         * if you won't have to change the texture throughout
         * the lifetime of the object.
         * 
         * Scaling is applied immediately and is not retained throughout
         * different texture handles (i.e. the texture's size
         * will not change if for whatever reason
         * the texture handle is changed and its original size changes).
         * 
         * @param scale new scale
         */
        void scale(float scale) {
            this->targetPortion.w = (int)(scale * (float)this->textureHandle.originalSize.width);
            this->targetPortion.h = (int)(scale * (float)this->textureHandle.originalSize.height);
        }
        /**
         * @brief Scales the texture separately in the
         * X and Y axes.
         * 
         * You can safely ignore the following information 
         * if you won't have to change the texture throughout
         * the lifetime of the object.
         * 
         * Scaling is applied immediately and is not retained throughout
         * different texture handles (i.e. the texture's size
         * will not change if for whatever reason
         * the texture handle is changed and its original size changes).
         * 
         * @param scaleX new horizontal scale
         * @param scaleY new vertical scale
         */
        void scale(float scaleX, float scaleY) {
            this->targetPortion.w = (int)(scaleX * (float)this->textureHandle.originalSize.width);
            this->targetPortion.h = (int)(scaleY * (float)this->textureHandle.originalSize.height);
        }
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
 * 
 */
class SoundEffect {
    friend class ResourceManager;

    private:
        Mix_Chunk* soundEffect = nullptr;
        uint32_t volume = 128;
    public:
        void setVolume(uint32_t newVolume) {
            Mix_VolumeChunk(this->soundEffect, newVolume);
            this->volume = newVolume <= 128 ? newVolume : 128;
        }
        uint32_t getVolume() const { return this->volume; }

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
 * 
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