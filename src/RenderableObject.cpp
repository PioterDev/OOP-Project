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
    const TextureHandle textureHandle
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

RenderableObject& RenderableObject::scaleX(const float scale) {
    if(scale < 0.0) return *this;
    this->targetPortion.w = (int)(scale * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).width);

    return *this;
}

RenderableObject& RenderableObject::scaleY(const float scale) {
    if(scale < 0.0) return *this;
    this->targetPortion.h = (int)(scale * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).height);

    return *this;
}

RenderableObject& RenderableObject::scale(const float scaleX, const float scaleY) {
    if(scaleX < 0.0) return *this;
    this->targetPortion.w = (int)(scaleX * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).width);
    if(scaleY < 0.0) return *this;
    this->targetPortion.h = (int)(scaleY * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).height);
    
    return *this;
}

RenderableObject& RenderableObject::scale(const float scale) {
    if(scale < 0.0) return *this;
    this->targetPortion.w = (int)(scale * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).width);
    this->targetPortion.h = (int)(scale * (float)Program::getResourceManager().getTextureOriginalSize(this->textureHandle).height);

    return *this;
}

RenderableObject& RenderableObject::rotate(const double degrees) {
    this->angle += degrees;
    while(this->angle >= 360.0) { this->angle -= 360.0; }
    while(this->angle <= 360.0) { this->angle += 360.0; }
    
    return *this;
}

RenderableObject& RenderableObject::setPositionOnScreen(const i32 x, const i32 y) {
    this->targetPortion.x = x;
    this->targetPortion.y = y;

    return *this;
}

RenderableObject& RenderableObject::setPositionOnScreenCentered(const i32 x, const i32 y) {
    this->targetPortion.x = x - this->targetPortion.w / 2;
    this->targetPortion.y = y - this->targetPortion.h / 2;

    return *this;
}

//TODO: to be reworked to allow querying the window size statically
/* RenderableObject& RenderableObject::setPositionOnScreenRelative(Program& program, float pX, float pY) {
    this->targetPortion.x = 
}

RenderableObject& RenderableObject::setPositionOnScreenCenteredRelative(float pX, float pY) {

} */

RenderableObject& RenderableObject::setSizeOnScreen(const u32 width, const u32 height) {
    this->targetPortion.w = (int)width;
    this->targetPortion.h = (int)height;

    return *this;
}

RenderableObject& RenderableObject::setSizeOnScreen(const Size size) {
    this->targetPortion.w = (int)size.width;
    this->targetPortion.h = (int)size.height;

    return *this;
}

RenderableObject& RenderableObject::moveOnScreen(const i32 dx, const i32 dy) {
    this->targetPortion.x += dx;
    this->targetPortion.y += dy;

    return *this;
}

RenderableObject& RenderableObject::stretchX(const float factor) {
    if(factor < 0.0) return *this;
    this->targetPortion.w = (int)roundf((float)this->targetPortion.w * factor);

    return *this;
}

RenderableObject& RenderableObject::stretchY(const float factor) {
    if(factor < 0.0) return *this;
    this->targetPortion.h = (int)roundf((float)this->targetPortion.h * factor);

    return *this;
}

RenderableObject& RenderableObject::stretch(const float factorX, const float factorY) {
    if(factorX < 0.0) return *this;
    this->targetPortion.w = (int)roundf((float)this->targetPortion.w * factorX);
    if(factorY < 0.0) return *this;
    this->targetPortion.h = (int)roundf((float)this->targetPortion.h * factorY);

    return *this;
}


RenderableObject& RenderableObject::stretch(const float factor) {
    if(factor < 0.0) return *this;
    this->targetPortion.w = (int)roundf((float)this->targetPortion.w * factor);
    this->targetPortion.h = (int)roundf((float)this->targetPortion.h * factor);

    return *this;
}


RenderableObject& RenderableObject::stretchX(const i32 dx) {
    this->targetPortion.w += dx;
    if(this->targetPortion.w < 0) this->targetPortion.w = 0;

    return *this;
}

RenderableObject& RenderableObject::stretchY(const i32 dy) {
    this->targetPortion.h += dy;
    if(this->targetPortion.h < 0) this->targetPortion.h = 0;

    return *this;
}

RenderableObject& RenderableObject::stretch(const i32 dx, const i32 dy) {
    this->targetPortion.w += dx;
    this->targetPortion.h += dy;
    if(this->targetPortion.w < 0) this->targetPortion.w = 0;
    if(this->targetPortion.h < 0) this->targetPortion.h = 0;

    return *this;
}

RenderableObject& RenderableObject::stretch(const i32 d) {
    this->targetPortion.w += d;
    this->targetPortion.h += d;
    if(this->targetPortion.w < 0) this->targetPortion.w = 0;
    if(this->targetPortion.h < 0) this->targetPortion.h = 0;

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

RenderableObject& RenderableObject::setBlendMode(const SDL_BlendMode blendMode) {
    this->blendMode = blendMode;
    
    return *this;
}