#include <cstdio>
#include <cerrno>

#include "program.hpp"
#include "resources.hpp"
#include "util.hpp"

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

bool ResourceManager::isTextureHandleValid(const TextureHandle handle) { return handle < this->textures.size(); }
bool ResourceManager::isFontHandleValid(const FontHandle handle) { return handle < this->fonts.size(); }

Status ResourceManager::init() {
    this->textures.reserve(128);
    this->soundEffects.reserve(16);
    this->music.reserve(4);
    this->fonts.reserve(2);
    TextureData t;
    t.texture = this->createFallbackTexture();
    if(t.texture == nullptr) {
        this->latestStatus = Status::FALLBACK_TEXTURE_CREATION_FAILURE;
        return Status::FALLBACK_TEXTURE_CREATION_FAILURE;
    }
    this->textures.push_back(t);
    memset((void*)&t, 0, sizeof(TextureData));
    this->textures.push_back(t); //no texture, placeholder for a transparent texture
    
    this->soundEffects.push_back(nullptr); //will be changed
    this->music.push_back(nullptr);
    this->fonts.push_back({nullptr, nullptr, 0});

    return Status::SUCCESS;
}

void ResourceManager::shutdown() {
    for(size_t i = 0; i < this->textures.size(); i++) {
        SDL_DestroyTexture(this->textures[i].texture);
        if(this->textures[i].flags & TextureFlags_CopyPath) {
            free((char*)this->textures[i].location);
        }
    }
    for(size_t i = 1; i < this->soundEffects.size(); i++) {
        Mix_FreeChunk(this->soundEffects[i]);
    }
    for(size_t i = 1; i < this->music.size(); i++) {
        Mix_FreeMusic(this->music[i]);
    }
    for(size_t i = 0; i < this->fonts.size(); i++) {
        TTF_CloseFont(this->fonts[i].font);
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
    if(t == nullptr) Program::getLogger().fatal("Cannot create a fallback texture: ", SDL_GetError());
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
        this->textures[handle].flags |= TextureFlags_LoadImmediately;

    }
    this->textures[handle].flags = flags;

    if(flags & TextureFlags_CopyPath) {
        size_t s = strlen(path) + 1;
        char* pathCopy = (char*)malloc(s);
        if(pathCopy != nullptr) {
            memcpy((void*)this->textures[handle].location, (void*)path, s);
        }
        this->textures[handle].location = pathCopy;
        this->textures[handle].flags |= TextureFlags_CopyPath;
    }
    else {
        this->textures[handle].location = path;
    }

    this->textures[handle].milisecondsToUnload = maxTimeLoad;
    this->textures[handle].lastAccessedAt = SDL_GetPerformanceCounter();
    
    return handle;
}

TextureHandle ResourceManager::__createTextTexture(
    const void* text, const u32 flags, const FontHandle font,
    const Color foregroundColor, const u32 wrapLength, const TextEncoding encoding
) {
    TextureHandle handle = (TextureHandle)this->textures.size();
    this->textures.emplace_back();
    
    switch (encoding) {
        case TextEncoding::UTF8:
            this->textures[handle].flags |= TextureFlags_Text_UTF8;
            break;
        case TextEncoding::UTF16:
            this->textures[handle].flags |= TextureFlags_Text_UTF16;
            break;
        default:
            break;
    }

    if(flags & TextureFlags_CopyPath) {
        size_t s;
        switch(encoding) {
            case TextEncoding::UTF8:
                s = strlen((const char*)text) + 1;
                break;
            case TextEncoding::UTF16:
                s = (wstrlen((const char16_t*)text) + 1) * 2;
                break;
            default:
                break;
        }
        char* textCopy = (char*)malloc(s);
        if(textCopy != nullptr) {
            memcpy((void*)this->textures[handle].location, (void*)text, s);
        }
        this->textures[handle].location = textCopy;
        this->textures[handle].flags |= TextureFlags_CopyPath;
    }
    else {
        this->textures[handle].location = (const char*)text;
    }
    //will decide later whether it makes sense to destroy a text box texture
    this->textures[handle].milisecondsToUnload = 0;
    this->textures[handle].lastAccessedAt = SDL_GetPerformanceCounter();

    SDL_Surface* s = nullptr;
    SDL_Texture* t = nullptr;

    TTF_Font* f = this->getFont(font);
    if(f == nullptr) goto failure;

    switch(encoding) {
        case TextEncoding::UTF8:
            s = TTF_RenderUTF8_Blended_Wrapped(
                f, (const char*)text, *(SDL_Color*)&foregroundColor, wrapLength
            );
            break;
        case TextEncoding::UTF16:
            s = TTF_RenderUNICODE_Blended_Wrapped(
                f, (const Uint16*)text, *(SDL_Color*)&foregroundColor, wrapLength
            );
            break;
        default:
            break;
        
    }
    if(s == nullptr) goto failure;
    
    t = SDL_CreateTextureFromSurface(Program::getRenderingContext(), s);
    SDL_FreeSurface(s);
    if(t == nullptr) goto failure;

    this->textures[handle].texture = t;

    return handle;

    failure: {
        Program::getLogger().error("Cannot create a text texture: ", TTF_GetError());
        if(flags & TextureFlags_CopyPath) {
            free((char*)this->textures[handle].location);
        }
        this->textures.pop_back();
        return 0;
    }
}

Status ResourceManager::loadTexture(TextureHandle handle) {
    if(handle >= this->textures.size()) return this->latestStatus = Status::OUT_OF_BOUNDS;
    if(this->textures[handle].location == nullptr) return this->latestStatus = Status::NULL_PASSED;

    if((this->latestStatus = tryOpeningFile(this->textures[handle].location)) != Status::SUCCESS) {
        return this->latestStatus;
    }
    
    if(this->textures[handle].texture != nullptr) return this->latestStatus = Status::ALREADY_EXISTS;

    this->textures[handle].texture = IMG_LoadTexture(
        Program::getRenderingContext(), this->textures[handle].location
    );
    if(this->textures[handle].texture == nullptr) return this->latestStatus = Status::TEXTURE_LOAD_FAILURE;

    return Status::SUCCESS;
}

SDL_Texture* ResourceManager::getTexture(TextureHandle handle) {
    if(!this->isTextureHandleValid(handle)) return this->textures[0].texture;
    this->textures[handle].lastAccessedAt = SDL_GetPerformanceCounter();
    return this->textures[handle].texture;
}

Size ResourceManager::getTextureOriginalSize(TextureHandle handle) {
    if(!this->isTextureHandleValid(handle)) return {fallbackTextureWidth, fallbackTextureHeight}; //fallback texture
    Size textureSize;
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

u32 ResourceManager::loadSoundEffect(const char* path) {
    Program::getLogger().info("Loading sound effect from ", path);
    if((this->latestStatus = tryOpeningFile(path)) != Status::SUCCESS) return 0;

    Mix_Chunk* chunk = Mix_LoadWAV(path);
    if(chunk == nullptr) {
        Program::getLogger().error("Cannot load sound effect: ", Mix_GetError());
        this->latestStatus = Status::SOUND_LOAD_FAILURE;
        return 0;
    }
    this->soundEffects.push_back(chunk);
    return this->soundEffects.size() - 1;
}

Mix_Chunk* ResourceManager::getSoundEffect(u32 id) {
    if(id >= this->soundEffects.size()) return nullptr;
    return this->soundEffects[id];
}


u32 ResourceManager::loadMusic(const char* path) {
    Program::getLogger().info("Loading music from ", path);
    if((this->latestStatus = tryOpeningFile(path)) != Status::SUCCESS) return 0;

    Mix_Music* music = Mix_LoadMUS(path);
    if(music == nullptr) {
        Program::getLogger().error("Cannot load music effect: ", Mix_GetError());
        this->latestStatus = Status::MUSIC_LOAD_FAILURE;
        return 0;
    }
    this->music.push_back(music);
    return this->music.size() - 1;
}

Mix_Music* ResourceManager::getMusic(u32 id) {
    if(id >= this->music.size()) return nullptr;
    return this->music[id];
}


FontHandle ResourceManager::loadFont(const char* path, const FontAttributes attributes) {
    Program::getLogger().info("Loading font from ", path);
    if((this->latestStatus = tryOpeningFile(path)) != Status::SUCCESS) return 0;

    FontData fontData = {nullptr, nullptr, 0};
    fontData.font = TTF_OpenFont(path, attributes.size & 0xFFFF);
    if(fontData.font == nullptr) {
        Program::getLogger().error("Cannot load font: ", TTF_GetError());
        this->latestStatus = Status::FONT_LOAD_FAILURE;
        return 0;
    }
    
    TTF_SetFontStyle(fontData.font, static_cast<int>(attributes.style));
    TTF_SetFontDirection(fontData.font, static_cast<TTF_Direction>(attributes.direction));
    TTF_SetFontWrappedAlign(fontData.font, static_cast<u32>(attributes.wrapAlignment));

    fontData.location = path;
    fontData.properties |= static_cast<u32>(attributes.style);
    fontData.properties |= static_cast<u32>(attributes.direction) << 4;
    fontData.properties |= static_cast<u32>(attributes.wrapAlignment) << 6;
    fontData.properties |= (attributes.size & 0xFFFF) << 8;

    this->fonts.push_back(fontData);

    return this->fonts.size() - 1;
}

TTF_Font* ResourceManager::getFont(FontHandle id) {
    if(!this->isFontHandleValid(id)) return nullptr;
    return this->fonts[id].font;
}

u32 ResourceManager::queryFontSize(FontHandle id) {
    if(!this->isFontHandleValid(id)) return 0;
    return (this->fonts[id].properties & (0xFFFF << 8)) >> 8;
}

FontStyle ResourceManager::queryFontStyle(FontHandle id) {
    if(!this->isFontHandleValid(id)) return FontStyle::Invalid;
    switch(this->fonts[id].properties & 0xF) {
        case 0:
            return FontStyle::Normal;
        case 1:
            return FontStyle::Bold;
        case 2:
            return FontStyle::Italic;
        case 4:
            return FontStyle::Underline;
        case 8:
            return FontStyle::Strikethrough;
        default:
            return FontStyle::Invalid; //should be unreachable
    }
}

FontDirection ResourceManager::queryFontDirection(FontHandle id) {
    if(!this->isFontHandleValid(id)) return FontDirection::Invalid;
    switch((this->fonts[id].properties & (0b11 << 4)) >> 4) {
        case 0:
            return FontDirection::LR;
        case 1:
            return FontDirection::RL;
        case 2:
            return FontDirection::TB;
        case 3:
            return FontDirection::BT;
        default:
            return FontDirection::Invalid; //should be unreachable
    }
}

FontWrapAlignment ResourceManager::queryFontWrapAlignment(FontHandle id) {
    if(!this->isFontHandleValid(id)) return FontWrapAlignment::Invalid;
    switch((this->fonts[id].properties & (0b11 << 6)) >> 6) {
        case 0:
            return FontWrapAlignment::Left;
        case 1:
            return FontWrapAlignment::Center;
        case 2:
            return FontWrapAlignment::Right;
        default:
            return FontWrapAlignment::Invalid; //should be unreachable
    }
}

FontAttributes ResourceManager::queryFontAttributes(FontHandle id) {
    FontAttributes att;
    att.style = this->queryFontStyle(id);
    att.direction = this->queryFontDirection(id);
    att.wrapAlignment = this->queryFontWrapAlignment(id);
    att.size = this->queryFontSize(id);

    return att;
}