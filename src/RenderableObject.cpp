#include "Game/Render/RenderableObject.hpp"

RenderableObjectBase::RenderableObjectBase(
    const u32 objectID
) : GameObject(objectID) {
    this->setVisible();
}

RenderableObjectBase::RenderableObjectBase(
    const u32 objectID, const char* name
) : GameObject(objectID, name) {
    this->setVisible();
}

RenderableObjectBase::RenderableObjectBase(
    const u32 objectID,
    const TextureHandle textureHandle
) : GameObject(objectID), textureHandle(textureHandle) {
    this->setVisible();
}

RenderableObjectBase::RenderableObjectBase(
    const u32 objectID,
    const char* name,
    const TextureHandle textureHandle
) : GameObject(objectID, name), textureHandle(textureHandle)  {
    this->setVisible();
}

RenderableObjectBase& RenderableObjectBase::bindTexture(
    TextureHandle textureHandle
) {
    this->textureHandle = textureHandle;
    this->setVisible();
    return *this;
}

RenderableObject::RenderableObject (
    const u32 objectID
) : RenderableObjectBase(objectID) {}

RenderableObject::RenderableObject(
    const u32 objectID,
    const char* name
) : RenderableObjectBase(objectID, name) {}

RenderableObject::RenderableObject(
    const u32 objectID,
    const TextureHandle textureHandle
) : RenderableObjectBase(objectID, textureHandle) {
    this->setTexturePortionOriginal();
}

RenderableObject::RenderableObject(
    const u32 objectID,
    const char* name,
    const TextureHandle textureHandle
) : RenderableObjectBase(objectID, name, textureHandle) {
    this->setTexturePortionOriginal();
}

RenderableObject& RenderableObject::setTexturePortion(const SDL_Rect& r) {
    this->texturePortion = r;
    return *this;
}

RenderableObject& RenderableObject::setTexturePortionOriginal() {
    this->texturePortion.x = this->texturePortion.y = 0;
    Size s = Program::getResourceManager().getTextureOriginalSize(this->textureHandle);
    this->texturePortion.w = (int)s.width;
    this->texturePortion.h = (int)s.height;
    return *this;
}

RenderableObject& RenderableObject::setTargetPortion(const SDL_Rect& r) {
    this->targetPortion = r;
    return *this;
}

RenderableObject& RenderableObject::scaleX(float scale) {
    this->targetPortion.w = (int)(scale * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).width);

    return *this;
}

RenderableObject& RenderableObject::scaleY(float scale) {
    this->targetPortion.h = (int)(scale * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).height);

    return *this;
}

RenderableObject& RenderableObject::scale(float scale) {
    this->targetPortion.w = (int)(scale * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).width);
    this->targetPortion.h = (int)(scale * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).height);

    return *this;
}

RenderableObject& RenderableObject::scale(float scaleX, float scaleY) {
    this->targetPortion.w = (int)(scaleX * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).width);
    this->targetPortion.h = (int)(scaleY * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).height);
    
    return *this;
}

RenderableObject& RenderableObject::rotate(double degrees) {
    this->angle += degrees;
    if(this->angle > 360.0) {
        this->angle -= 360.0;
    }
    return *this;
}

RenderableObject& RenderableObject::setPositionOnScreen(int x, int y) {
    this->targetPortion.x = x;
    this->targetPortion.y = y;
    return *this;
}

RenderableObject& RenderableObject::setPositionOnScreenCentered(int x, int y) {
    this->targetPortion.x = x - this->targetPortion.w / 2;
    this->targetPortion.y = y - this->targetPortion.h / 2;
    return *this;
}

RenderableObject& RenderableObject::setSizeOnScreen(u32 width, u32 height) {
    this->targetPortion.w = (int)width;
    this->targetPortion.h = (int)height;
    return *this;
}

RenderableObject& RenderableObject::unflip() {
    this->flip = SDL_FLIP_NONE;
    return *this;
}

RenderableObject& RenderableObject::flipHorizontally() {
    if(this->flip == SDL_FLIP_NONE) this->flip = SDL_FLIP_HORIZONTAL;
    else if(this->flip == SDL_FLIP_HORIZONTAL) this->flip = SDL_FLIP_NONE;
    return *this;
}

RenderableObject& RenderableObject::flipVertically() {
    if(this->flip == SDL_FLIP_NONE) this->flip = SDL_FLIP_VERTICAL;
    else if(this->flip == SDL_FLIP_VERTICAL) this->flip = SDL_FLIP_NONE;
    return *this;
}

RenderableObject& RenderableObject::setModulation(
    const u8 red, const u8 green, const u8 blue, const u8 alpha
) {
    this->colorModulation.red    = red;
    this->colorModulation.green  = green;
    this->colorModulation.blue   = blue;
    this->colorModulation.alpha  = alpha;
    return *this;
}

RenderableObject& RenderableObject::setModulation(const u32 rgba) {
    *(u32*)(&this->colorModulation) = rgba; //this is super hacky and may not work on big endian systems, but who cares
    return *this;
}

RenderableObject& RenderableObject::setModulationRed(const u8 mod) {
    this->colorModulation.red = mod;
    return *this;
}

RenderableObject& RenderableObject::setModulationGreen(const u8 mod) {
    this->colorModulation.green = mod;
    return *this;
}

RenderableObject& RenderableObject::setModulationBlue(const u8 mod) {
    this->colorModulation.blue = mod;
    return *this;
}

RenderableObject& RenderableObject::setModulationAlpha(const u8 mod) {
    this->colorModulation.alpha = mod;
    return *this;
}

RenderableObject& RenderableObject::setBlendNone() {
    this->blendMode = SDL_BLENDMODE_NONE;
    return *this;
}

RenderableObject& RenderableObject::setBlendAlpha() {
    this->blendMode = SDL_BLENDMODE_BLEND;
    return *this;
}

RenderableObject& RenderableObject::setBlendAdditive() {
    this->blendMode = SDL_BLENDMODE_ADD;
    return *this;
}

RenderableObject& RenderableObject::setBlendModulate() {
    this->blendMode = SDL_BLENDMODE_MOD;
    return *this;
}

RenderableObject& RenderableObject::setBlendMultiplicative() {
    this->blendMode = SDL_BLENDMODE_MUL;
    return *this;
}

RenderableObject& RenderableObject::setBlendMode(SDL_BlendMode blendMode) {
    this->blendMode = blendMode;
    return *this;
}