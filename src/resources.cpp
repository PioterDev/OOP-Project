#include <cstdio>
#include <cerrno>

#include "program.hpp"
#include "resources.hpp"

static constexpr u32 fallbackTextureWidth = 64;
static constexpr u32 fallbackTextureHeight = 64;

static constexpr TextureHandle fallbackHandle = 0;

Status tryOpeningFile(const char* path) {
    if(path == nullptr) return Status::FAILURE;
    FILE* f = fopen(path, "r");
    if(f != nullptr) {
        fclose(f);
        return Status::SUCCESS;
    }
    else {
        switch(errno) {
            case EACCES:
                Program::getLogger().error("Cannot load texture at ", path, ": access denied");
                return Status::ACCESS_DENIED;
            case ENOENT:
                Program::getLogger().error("Cannot load texture at ", path, ": file does not exist");
                return Status::NONEXISTENT;
            default:
                Program::getLogger().error("Cannot load texture at ", path, ": ", strerror(errno));
                return Status::FAILURE;
        }
    }
}

Status ResourceManager::init() {
    textures.reserve(128);
    soundEffects.reserve(16);
    music.reserve(4);
    TextureData t;
    t.texture = this->createFallbackTexture();
    if(t.texture == nullptr) {
        this->latestStatus = Status::FALLBACK_TEXTURE_CREATION_FAILURE;
        return Status::FALLBACK_TEXTURE_CREATION_FAILURE;
    }
    textures.push_back(t);
    memset((void*)&t, 0, sizeof(TextureData));
    textures.push_back(t); //no texture, placeholder for a transparent texture
    
    soundEffects.push_back(nullptr); //will be changed
    music.push_back(nullptr);

    return Status::SUCCESS;
}

void ResourceManager::shutdown() {
    for(size_t i = 0; i < this->textures.size(); i++) {
        SDL_DestroyTexture(this->textures[i].texture);
        if(this->textures[i].flags & TextureFlags_CopyPath) {
            delete[] this->textures[i].location;
        }
    }
    for(size_t i = 1; i < this->soundEffects.size(); i++) {
        Mix_FreeChunk(this->soundEffects[i]);
    }
    for(size_t i = 1; i < this->music.size(); i++) {
        Mix_FreeMusic(this->music[i]);
    }
}

SDL_Texture* ResourceManager::createFallbackTexture() {
    SDL_Rect rect = {0, 0, (int)fallbackTextureWidth, (int)fallbackTextureHeight};
    SDL_Surface* s = SDL_CreateRGBSurface(
        0, (int)fallbackTextureWidth, (int)fallbackTextureHeight,
        32, 0, 0, 0, 0
    );
    if(s == nullptr) return nullptr;
    
    if(SDL_FillRect(s, &rect, SDL_MapRGB(s->format, 255, 0, 255))) {
        SDL_FreeSurface(s);
        Program::getLogger().fatal("Cannot create a fallback texture: ", SDL_GetError());
        return nullptr;
    }
    //the rest *should* not fail

    rect.x += 32;
    SDL_FillRect(s, &rect, 0);
    rect.x = 0;
    rect.y += 32;
    SDL_FillRect(s, &rect, 0);
    rect.x += 32;
    SDL_FillRect(s, &rect, SDL_MapRGB(s->format, 255, 0, 255));

    SDL_Texture* t = SDL_CreateTextureFromSurface(
        Program::getRenderingContext(), s
    );
    SDL_FreeSurface(s);
    if(t == nullptr) {
        Program::getLogger().fatal("Cannot create a fallback texture: ", SDL_GetError());
    }
    return t;

}

TextureHandle ResourceManager::registerTexture(const char* path, const u32 flags, const u32 maxTimeLoad) {
    
    Program::getLogger().info("Registering texture at ", path);
    
    if((this->latestStatus = tryOpeningFile(path)) != Status::SUCCESS) return fallbackHandle;
    
    TextureHandle handle = (TextureHandle)this->textures.size();
    this->textures.emplace_back();
    
    if(flags & TextureFlags_LoadImmediately) {
        this->textures[handle].texture = IMG_LoadTexture(Program::getRenderingContext(), path);
        if(this->textures[handle].texture == nullptr) {
            Program::getLogger().error("Cannot load texture: ", IMG_GetError());
            this->latestStatus = Status::TEXTURE_LOAD_FAILURE;
            this->textures.pop_back();
            return fallbackHandle;
        }
        SDL_SetTextureScaleMode(this->textures[handle].texture, SDL_ScaleModeBest);

    }
    this->textures[handle].flags = flags;

    if(flags & TextureFlags_CopyPath) {
        size_t s = strlen(path) + 1;
        this->textures[handle].location = new char[s];
        memcpy((void*)this->textures[handle].location, (void*)path, s);
    }
    else {
        this->textures[handle].location = path;
    }

    this->textures[handle].milisecondsToUnload = maxTimeLoad;
    this->textures[handle].lastAccessedAt = SDL_GetPerformanceCounter();
    
    return handle;
}

Status ResourceManager::loadTexture(TextureHandle handle) {
    if(handle >= this->textures.size()) {
        this->latestStatus = Status::OUT_OF_BOUNDS;
        return Status::OUT_OF_BOUNDS;
    }
    if(this->textures[handle].location == nullptr) { 
        this->latestStatus = Status::NULL_PASSED;
        return Status::NULL_PASSED;
    }

    Status tryOpen = tryOpeningFile(this->textures[handle].location);
    if(tryOpen != Status::SUCCESS) {
        this->latestStatus = tryOpen;
        return tryOpen;
    }
    
    if(this->textures[handle].texture != nullptr) {
        this->latestStatus = Status::ALREADY_EXISTS;
        return Status::ALREADY_EXISTS;
    }

    this->textures[handle].texture = IMG_LoadTexture(
        Program::getRenderingContext(), this->textures[handle].location
    );
    if(this->textures[handle].texture == nullptr) {
        this->latestStatus = Status::TEXTURE_LOAD_FAILURE;
        return Status::TEXTURE_LOAD_FAILURE;
    }

    return Status::SUCCESS;
}

SDL_Texture* ResourceManager::getTexture(TextureHandle handle) {
    if(handle >= this->textures.size()) return this->textures[0].texture;
    this->textures[handle].lastAccessedAt = SDL_GetPerformanceCounter();
    return this->textures[handle].texture;
}

Size ResourceManager::getTextureOriginalSize(TextureHandle handle) {
    Size textureSize;
    if(handle >= this->textures.size()) return {fallbackTextureWidth, fallbackTextureHeight}; //fallback texture
    if(this->textures[handle].texture == nullptr) {
        this->latestStatus = Status::NULL_PASSED;
        return {0, 0};
    }
    if(SDL_QueryTexture(
        this->textures[handle].texture, nullptr, nullptr,
        (int*)&textureSize.width, (int*)&textureSize.height)
    ) {
        Program::getLogger().error("Cannot query texture: ", SDL_GetError());
        this->latestStatus = Status::TEXTURE_QUERY_FAILURE;
        return {0, 0};
    }
    this->textures[handle].lastAccessedAt = SDL_GetPerformanceCounter();
    return textureSize;
}

Status SoundEffect::loadSoundEffect(const char* path) {
    Status s = tryOpeningFile(path);
    if(s == Status::SUCCESS) {
        this->soundEffect = Mix_LoadWAV(path);
        if(this->soundEffect == nullptr) {
            Program::getLogger().error("Cannot load sound effect: ", Mix_GetError());
            return Status::SOUND_LOAD_FAILURE;
        }
        return Status::SUCCESS;
    }
    else return s;

}

Status Music::loadMusic(const char* path) {
    Status s = tryOpeningFile(path);
    if(s == Status::SUCCESS) {
        this->music = Mix_LoadMUS(path);
        if(this->music == nullptr) {
            Program::getLogger().error("Cannot load music: ", Mix_GetError());
            return Status::MUSIC_LOAD_FAILURE;
        }
        return Status::SUCCESS;
    }
    else return s;
}