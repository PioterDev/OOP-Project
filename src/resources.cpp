#include <cstdio>
#include <cerrno>

#include "program.hpp"
#include "resources.hpp"

static constexpr uint32_t fallbackTextureWidth = 64;
static constexpr uint32_t fallbackTextureHeight = 64;

Status tryOpeningFile(const char* path) {
    FILE* f = fopen(path, "r");
    if(f != NULL) {
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

ResourceManager::~ResourceManager()  {
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

Status ResourceManager::init() {
    textures.reserve(128);
    soundEffects.reserve(16);
    music.reserve(4);
    TextureHandle t;
    t.texture = this->createFallbackTexture();
    if(t.texture == nullptr) {
        this->latestStatus = Status::FALLBACK_TEXTURE_CREATION_FAILURE;
        return Status::FALLBACK_TEXTURE_CREATION_FAILURE;
    }
    t.originalSize.width = fallbackTextureWidth;
    t.originalSize.height = fallbackTextureHeight;
    textures.push_back(t);
    memset((void*)&t, 0, sizeof(TextureHandle));
    textures.push_back(t); //no texture, placeholder for a transparent texture
    
    soundEffects[0] = nullptr;
    music[0] = nullptr;

    return Status::SUCCESS;
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

uint32_t ResourceManager::registerTexture(const char* path) {
    Program::getLogger().info("Registering texture at ", path);

    this->latestStatus = tryOpeningFile(path);
    if(this->latestStatus == Status::SUCCESS) {
        TextureHandle t;
        t.texture = IMG_LoadTexture(Program::getRenderingContext(), path);
        if(t.texture == nullptr) {
            Program::getLogger().error("Cannot load texture: ", IMG_GetError());
            this->latestStatus = Status::TEXTURE_LOAD_FAILURE;
            return 0;
        }
        SDL_SetTextureScaleMode(t.texture, SDL_ScaleModeBest);
        if(SDL_QueryTexture(
            t.texture, nullptr, nullptr, 
            (int*)&t.originalSize.width,
            (int*)&t.originalSize.height)
        ) {
            Program::getLogger().error("Failed to query texture: ", SDL_GetError());
            this->latestStatus=  Status::TEXTURE_QUERY_FAILURE;
            return 0;
        }
        this->textures.push_back(t);
        return this->textures.size() - 1;
    }
    else return 0;
}

TextureHandle ResourceManager::getTextureHandle(uint32_t id) const {
    if(id > this->textures.size()) return this->textures[0]; //fallback texture
    return this->textures[id];
}

SDL_Texture* ResourceManager::getTexture(uint32_t id) {
    if(id > this->textures.size()) return this->textures[0].texture;
    return this->textures[id].texture;
}

Size ResourceManager::getTextureOriginalSize(uint32_t id) const {
    if(id > this->textures.size()) return this->textures[0].originalSize; //fallback texture
    return this->textures[id].originalSize;
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