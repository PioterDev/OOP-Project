#include <cstdio>
#include <cerrno>

#include "program.hpp"
#include "resources.hpp"
#include "util.hpp"

using namespace Enums;
using namespace Structs;

static constexpr Size fallbackTextureSize = {64, 64};

static constexpr TextureHandle fallbackHandle = 0;


const char* unknownError = "Cannot easily determine the cause of an error, sorry :(";
const char* noErrorCString = "There was no error";
const char* texHandleOOB = "Texture handle out of bounds";
const char* texAlreadyExists = "Texture in the given handle already exists";
const char* invalidTexHandle = "Invalid texture handle";
const char* OOM = "Out of memory";



Status tryOpeningFile(const char* path) noexcept {
    if(path == nullptr) return Status::NULL_PASSED;
    FILE* f = fopen(path, "r");
    if(f) { fclose(f); return Status::SUCCESS; }
    else {
        switch(errno) {
            case EACCES: return Status::ACCESS_DENIED;
            case ENOENT: return Status::NONEXISTENT;
            default: return Status::FAILURE;
        }
    }
}

static const char* getFileOpenErrorMessage(Status status) noexcept {
    switch(status) {
        case Status::NULL_PASSED:   return "File path was NULL";
        case Status::ACCESS_DENIED: return "Opening file not permitted";
        case Status::NONEXISTENT:   return "File does not exist";
        default: return strerror(errno);
    }
}



bool ResourceManager::isTextureHandleValid(const TextureHandle handle) const noexcept {
    if(handle >= this->textures.size()) return false;
    else if(!(this->textures[handle].flags & TextureFlags_IsValid)) return false;
    return true;
}

bool ResourceManager::isFontHandleValid(const FontHandle handle) const noexcept {
    return handle < this->fonts.size();
}


SDL_Texture* ResourceManager::__createFallbackTexture() noexcept {
    SDL_Rect rect = {0, 0, (int)fallbackTextureSize.width, (int)fallbackTextureSize.height};
    SDL_Surface* s = SDL_CreateRGBSurface(
        0, (int)fallbackTextureSize.width, (int)fallbackTextureSize.height,
        32, 0, 0, 0, 0
    );
    if(s == nullptr) Unlikely {
        this->latestStatus = Status::SDL_SURFACE_CREATION_FAILURE;
        this->errorMessage = SDL_GetError();
        return nullptr;
    }
    
    SDL_Texture* t = nullptr;
    
    if(SDL_FillRect(
        s, &rect, SDL_MapRGB(s->format, 255, 0, 255)
    )) Unlikely {
        goto failure;
    }

    rect.x += 32;
    if(SDL_FillRect(s, &rect, 0)) Unlikely {
        goto failure;
    }

    rect.x = 0;
    rect.y += 32;
    if(SDL_FillRect(s, &rect, 0)) Unlikely {
        goto failure;
    }

    rect.x += 32;
    if(SDL_FillRect(
        s, &rect, SDL_MapRGB(s->format, 255, 0, 255)
    )) Unlikely {
        goto failure;
    }

    t = SDL_CreateTextureFromSurface(Program::getRenderingContext(), s);
    if(t == nullptr) goto failure;
    SDL_FreeSurface(s);
    return t;

    failure:
        SDL_FreeSurface(s);
        this->latestStatus = Status::SDL_FAILURE;
        this->errorMessage = SDL_GetError();
        return nullptr;
}

Status ResourceManager::__registerTextureAt(
    TextureHandle handle, const char* path, const u32 flags
) noexcept {
    this->latestStatus = Status::SUCCESS;
    this->errorMessage = noErrorCString;
    
    if(handle >= this->textures.size()) {
        this->errorMessage = texHandleOOB;
        return this->latestStatus = Status::OUT_OF_BOUNDS;
    }

    TextureData& data = this->textures[handle];

    if(data.flags & TextureFlags_IsValid) {
        this->errorMessage = texAlreadyExists;
        return this->latestStatus = Status::ALREADY_EXISTS;
    }

    u32 scaleMode = (flags & (0b11 << 4)) >> 4;
    if(scaleMode > SDL_ScaleModeBest) scaleMode = SDL_ScaleModeBest;
    data.flags |= scaleMode << 4; //lots of shifting, lol
    
    if(flags & TextureFlags_LoadImmediately) {
        if((this->latestStatus = tryOpeningFile(path)) != Status::SUCCESS) {
            this->errorMessage = getFileOpenErrorMessage(this->latestStatus);
            return this->latestStatus;
        }
        data.texture = IMG_LoadTexture(Program::getRenderingContext(), path);
        if(data.texture == nullptr) {
            // Program::getLogger().error("Cannot load texture: ", IMG_GetError());
            this->errorMessage = IMG_GetError();
            return this->latestStatus = Status::SDL_IMAGE_LOADTEXTURE_FAILURE;
        }
        
        SDL_SetTextureScaleMode(data.texture, (SDL_ScaleMode)scaleMode);
        data.flags |= TextureFlags_LoadImmediately;
    }

    if(flags & TextureFlags_CopyPath) {
        size_t s = strlen(path) + 1;
        char* pathCopy = (char*)malloc(s);

        if(pathCopy) memcpy(pathCopy, path, s);
        else this->latestStatus = Status::ALLOC_FAILURE;

        data.location = pathCopy;
        data.flags |= TextureFlags_CopyPath;
    }
    else data.location = path;

    data.flags |= TextureFlags_IsValid;

    // this->textures[handle].lastAccessedAt = SDL_GetPerformanceCounter();
    return this->latestStatus;
}

TextureHandle ResourceManager::__createTextTexture(
    const void* text, const u32 flags, const FontHandle font,
    const Color foregroundColor, const u32 wrapLength, const TextEncoding encoding
) noexcept {
    TextureHandle handle = this->textures.size();
    this->textures.emplace_back();
    switch(this->__createTextTextureAt(
        handle, text, flags, font,
        foregroundColor, wrapLength, encoding
    )) {
        case Status::SUCCESS:
            return handle;
        case Status::ALLOC_FAILURE:
            this->errorMessage = 
                "Memory allocation for copying the text failed in "
                "ResourceManager::createTextTexture(...)";
            return handle;
        default:
            this->textures.pop_back();
            return fallbackHandle;
    }
}

Status ResourceManager::__createTextTextureAt(
    TextureHandle handle,
    const void* text, const u32 flags, const FontHandle font,
    const Color foregroundColor, const u32 wrapLength, const TextEncoding encoding
) noexcept {
    this->latestStatus = Status::SUCCESS;
    this->errorMessage = noErrorCString;

    if(handle >= this->textures.size()) {
        this->errorMessage = texHandleOOB;
        return this->latestStatus = Status::OUT_OF_BOUNDS;
    }

    TextureData& data = this->textures[handle];

    if(data.flags & TextureFlags_IsValid) {
        this->errorMessage = texAlreadyExists;
        return this->latestStatus = Status::ALREADY_EXISTS;
    }

    if(text == nullptr) {
        this->errorMessage = "Text variable passed was NULL";
        return this->latestStatus = Status::NULL_PASSED;
    }

    switch (encoding) {
        case TextEncoding::UTF8:
            data.flags |= TextureFlags_Text_UTF8;
            break;
        case TextEncoding::UTF16:
            data.flags |= TextureFlags_Text_UTF16;
            break;
        default:
            this->errorMessage = "Invalid/unsupported text encoding was somehow passed";
            return this->latestStatus = Status::INVALID_ARGS;
    }

    if(flags & TextureFlags_CopyPath) {
        size_t s = 0;
        switch(encoding) {
            case TextEncoding::UTF8:
                s = strlen((const char*)text) + 1;
                break;
            case TextEncoding::UTF16:
                s = (wstrlen((const char16_t*)text) + 1) * 2;
                break;
            default: //unreachable
                break;
        }
        char* textCopy = (char*)malloc(s);
        if(textCopy != nullptr) memcpy(textCopy, text, s);
        else this->latestStatus = Status::ALLOC_FAILURE;
        
        data.location = textCopy;
        data.flags |= TextureFlags_CopyPath;
    }
    else {
        //in the context of text textures, `location` is used
        //instead of another variable
        data.location = (const char*)text;
    }
    //will decide later whether it makes sense to destroy a text box texture
    data.milisecondsToUnload = 0;
    data.lastAccessedAt = SDL_GetPerformanceCounter();

    SDL_Surface* s = nullptr;
    SDL_Texture* t = nullptr;
    u32 scaleMode = (flags & (0b11 << 4)) >> 4;

    TTF_Font* f = this->getFont(font);
    if(f == nullptr) {
        this->latestStatus = Status::OUT_OF_BOUNDS;
        this->errorMessage = "Font handle out of bounds";
        goto failure;
    }

    switch(encoding) {
        case TextEncoding::UTF8:
            s = TTF_RenderUTF8_Blended_Wrapped(
                f, (const char*)text,
                *(SDL_Color*)&foregroundColor, wrapLength
            );
            break;
        case TextEncoding::UTF16:
            s = TTF_RenderUNICODE_Blended_Wrapped(
                f, (const Uint16*)text,
                *(SDL_Color*)&foregroundColor, wrapLength
            );
            break;
        default: //unreachable
            break;
        
    }
    if(s == nullptr) {
        this->latestStatus = Status::SDL_TTF_RENDERTEXT_FAILURE;
        this->errorMessage = TTF_GetError();
        goto failure;
    }
    
    t = SDL_CreateTextureFromSurface(Program::getRenderingContext(), s);
    SDL_FreeSurface(s);
    if(t == nullptr) {
        this->latestStatus = Status::SDL_TEXTURE_CREATION_FAILURE;
        this->errorMessage = SDL_GetError();
        goto failure;
    }

    if(scaleMode > SDL_ScaleModeBest) scaleMode = SDL_ScaleModeBest;
    SDL_SetTextureScaleMode(t, (SDL_ScaleMode)scaleMode);
    data.flags |= scaleMode << 4;

    data.texture = t;
    data.flags |= TextureFlags_IsValid;

    return this->latestStatus;

    failure: {
        if(flags & TextureFlags_CopyPath) {
            free((char*)data.location);
        }
        return this->latestStatus;
    }
}



Status ResourceManager::init() noexcept {
    this->textures.reserve(128);
    this->soundEffects.reserve(16);
    this->music.reserve(4);
    this->fonts.reserve(2);
    
    TextureData t = {this->__createFallbackTexture(), nullptr, 0, 0, 0};
    if(!t.texture) return this->latestStatus = Status::FALLBACK_TEXTURE_CREATION_FAILURE;
    t.flags |= TextureFlags_IsValid;
    this->textures.push_back(t);

    memset((void*)&t, 0, sizeof(TextureData));
    this->textures.push_back(t); //no texture, placeholder for a transparent texture
    
    this->soundEffects.push_back(nullptr); //will be changed
    this->music.push_back(nullptr);
    this->fonts.push_back({nullptr, nullptr, 0});

    return Status::SUCCESS;
}

void ResourceManager::shutdown() noexcept {
    for(size_t i = 0; i < this->textures.size(); i++) {
        if(this->textures[i].texture) SDL_DestroyTexture(this->textures[i].texture);
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

TextureHandle ResourceManager::registerTexture(const char* path, const u32 flags) noexcept {
    // Program::getLogger().info("Registering texture at ", path);
    
    TextureHandle handle = (TextureHandle)this->textures.size();
    try {
        this->textures.emplace_back();
    }
    catch(const std::bad_alloc&) {
        //TODO: make Program handle OOM
    }

    switch(this->__registerTextureAt(handle, path, flags)) {
        case Status::SUCCESS:
            return handle;
        //a very bad case, but won't cause a crashing state
        case Status::ALLOC_FAILURE:
            this->errorMessage = 
                "Memory allocation for copying the path failed in "
                "ResourceManager::registerTexture(...)";
            return handle;
        case Status::SDL_TEXTURE_QUERY_FAILURE:
            return handle;
        default:
            this->textures.pop_back();
            return fallbackHandle;
    }
}

TextureHandle ResourceManager::reserveTextureHandle() noexcept {
    TextureHandle handle = this->textures.size();
    try {
        this->textures.emplace_back();
    }
    catch(const std::bad_alloc&) {
        this->latestStatus = Status::ALLOC_FAILURE;
        this->errorMessage = OOM;
        return fallbackHandle;
    }
    return handle;
}

Status ResourceManager::loadTexture(TextureHandle handle) noexcept {
    if(!this->isTextureHandleValid(handle)) {
        this->errorMessage = invalidTexHandle;
        return this->latestStatus = Status::INVALID_ARGS;
    }

    TextureData& data = this->textures[handle];

    if(data.texture != nullptr) {
        //already loaded - we don't care if it's a text
        //texture, doesn't have a set location or
        //if its file even exists, it's loaded
        return Status::SUCCESS;
    }

    if(this->isTextTexture(handle)) {
        this->errorMessage = "Cannot load texture - it is a text texture";
        return this->latestStatus = Status::FAILURE;
    }

    if(data.location == nullptr) {
        this->errorMessage = "Texture doesn't have a set location";
        return this->latestStatus = Status::NULL_PASSED;
    }

    if((this->latestStatus = tryOpeningFile(data.location)) != Status::SUCCESS) {
        this->errorMessage = getFileOpenErrorMessage(this->latestStatus);
        return this->latestStatus;
    }
    
    data.texture = IMG_LoadTexture(Program::getRenderingContext(), data.location);
    if(data.texture == nullptr) {
        this->errorMessage = IMG_GetError();
        return this->latestStatus = Status::SDL_IMAGE_LOADTEXTURE_FAILURE;
    }

    SDL_SetTextureScaleMode(
        data.texture, (SDL_ScaleMode)((data.flags & (0b11 << 4)) >> 4)
    );
    

    return Status::SUCCESS;
}

Status ResourceManager::unloadTexture(TextureHandle handle) noexcept {
    if(!this->isTextureHandleValid(handle)) {
        this->errorMessage = invalidTexHandle;
        return this->latestStatus = Status::INVALID_ARGS;
    }

    if(this->textures[handle].texture != nullptr) {
        SDL_DestroyTexture(this->textures[handle].texture);
        this->textures[handle].texture = nullptr;
    }

    return Status::SUCCESS;
}

SDL_Texture* ResourceManager::getTexture(TextureHandle handle) noexcept {
    if(!this->isTextureHandleValid(handle)) return this->textures[0].texture;
    
    TextureData& data = this->textures[handle];
    if(data.texture == nullptr) {
        if(this->loadTexture(handle) != Status::SUCCESS) {
            return this->textures[0].texture;
        }
    }
    
    data.lastAccessedAt = SDL_GetPerformanceCounter();
    return data.texture;
}

Size ResourceManager::getTextureOriginalSize(TextureHandle handle) noexcept {
    if(!this->isTextureHandleValid(handle)) 
        return fallbackTextureSize;

    Size textureSize;
    TextureData& data = this->textures[handle];
    
    if(data.texture == nullptr) {
        if(this->loadTexture(handle) != Status::SUCCESS) {
            return {0, 0};
        }
    }

    if(SDL_QueryTexture(
        data.texture, nullptr, nullptr,
        (int*)&textureSize.width, (int*)&textureSize.height)
    ) {
        this->latestStatus = Status::SDL_TEXTURE_QUERY_FAILURE;
        this->errorMessage = SDL_GetError();
        return {0, 0};
    }
    data.lastAccessedAt = SDL_GetPerformanceCounter();

    return textureSize;
}

Status ResourceManager::destroyTexture(TextureHandle handle) noexcept {
    if(!this->isTextureHandleValid(handle)) {
        this->errorMessage = "Texture handle already invalid";
        return this->latestStatus = Status::NONEXISTENT;
    }

    TextureData& data = this->textures[handle];

    if((data.flags & TextureFlags_CopyPath)) {
        free((void*)data.location);
        data.flags ^= TextureFlags_CopyPath;
    }
    data.location = nullptr;
    if(data.texture != nullptr) {
        SDL_DestroyTexture(data.texture);
        data.texture = nullptr;
    }
    data.milisecondsToUnload = 0;
    data.flags = 0;

    return Status::SUCCESS;
}

bool ResourceManager::isTextTexture(TextureHandle handle) noexcept {
    if(!this->isTextureHandleValid(handle)) return false;

    return (this->textures[handle].flags & (TextureFlags_Text_UTF8 | TextureFlags_Text_UTF16)) != 0;
}

SFXHandle ResourceManager::loadSoundEffect(const char* path, u8 volume) noexcept {
    if((this->latestStatus = tryOpeningFile(path)) != Status::SUCCESS) {
        this->errorMessage = getFileOpenErrorMessage(this->latestStatus);
        return 0;
    }

    Mix_Chunk* chunk = Mix_LoadWAV(path);
    if(chunk == nullptr) {
        this->latestStatus = Status::SOUND_LOAD_FAILURE;
        this->errorMessage = Mix_GetError();
        return 0;
    }
    Mix_VolumeChunk(chunk, volume);

    this->soundEffects.push_back(chunk);
    return this->soundEffects.size() - 1;
}

Mix_Chunk* ResourceManager::getSoundEffect(SFXHandle id) noexcept {
    if(id >= this->soundEffects.size()) return nullptr;
    return this->soundEffects[id];
}


MusicHandle ResourceManager::loadMusic(const char* path) noexcept {
    Program::getLogger().info("Loading music from ", path);
    if((this->latestStatus = tryOpeningFile(path)) != Status::SUCCESS) {
        this->errorMessage = getFileOpenErrorMessage(this->latestStatus);
        return 0;
    }

    Mix_Music* music = Mix_LoadMUS(path);
    if(music == nullptr) {
        this->latestStatus = Status::MUSIC_LOAD_FAILURE;
        this->errorMessage = Mix_GetError();
        return 0;
    }
    this->music.push_back(music);
    return this->music.size() - 1;
}

Mix_Music* ResourceManager::getMusic(MusicHandle handle) noexcept {
    if(handle >= this->music.size()) return nullptr;
    return this->music[handle];
}


FontHandle ResourceManager::loadFont(const char* path, const FontAttributes attributes) noexcept {
    Program::getLogger().info("Loading font from ", path);
    if((this->latestStatus = tryOpeningFile(path)) != Status::SUCCESS) {
        this->errorMessage = getFileOpenErrorMessage(this->latestStatus);
        return 0;
    }
    FontHandle handle = this->fonts.size();
    FontData* fontData = nullptr;

    try {
        fontData = &this->fonts.emplace_back();
    }
    catch(const std::bad_alloc&) {
        //TODO: make Program handle OOM

        this->latestStatus = Status::ALLOC_FAILURE;
        this->errorMessage = "Font registry failed to reallocate";
    }

    // FontData fontData = {nullptr, nullptr, 0};
    fontData->font = TTF_OpenFont(path, attributes.size & 0xFFFF);
    if(fontData->font == nullptr) {
        Program::getLogger().error("Cannot load font: ", TTF_GetError());
        this->latestStatus = Status::FONT_LOAD_FAILURE;
        return 0;
    }
    
    TTF_SetFontStyle(fontData->font, static_cast<int>(attributes.style));
    TTF_SetFontDirection(fontData->font, static_cast<TTF_Direction>(attributes.direction));
    TTF_SetFontWrappedAlign(fontData->font, static_cast<u32>(attributes.wrapAlignment));

    fontData->location = path;
    fontData->properties |= static_cast<u32>(attributes.style);
    fontData->properties |= static_cast<u32>(attributes.direction) << 4;
    fontData->properties |= static_cast<u32>(attributes.wrapAlignment) << 6;
    fontData->properties |= (attributes.size & 0xFFFF) << 8;

    return handle;
}

TTF_Font* ResourceManager::getFont(FontHandle id) noexcept {
    if(!this->isFontHandleValid(id)) return nullptr;
    return this->fonts[id].font;
}

u32 ResourceManager::queryFontSize(FontHandle id) noexcept {
    if(!this->isFontHandleValid(id)) return 0;
    return (this->fonts[id].properties & (0xFFFF << 8)) >> 8;
}

FontStyle ResourceManager::queryFontStyle(FontHandle id) noexcept {
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

FontDirection ResourceManager::queryFontDirection(FontHandle id) noexcept {
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

FontWrapAlignment ResourceManager::queryFontWrapAlignment(FontHandle id) noexcept {
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

FontAttributes ResourceManager::queryFontAttributes(FontHandle id) noexcept {
    FontAttributes att;
    att.style = this->queryFontStyle(id);
    att.direction = this->queryFontDirection(id);
    att.wrapAlignment = this->queryFontWrapAlignment(id);
    att.size = this->queryFontSize(id);

    return att;
}