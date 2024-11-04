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
    const u32 textureHandleIndex
) : GameObject(objectID) {
    this->bindTexture(textureHandleIndex);
}

RenderableObjectBase::RenderableObjectBase(
    const u32 objectID,
    const char* name,
    const u32 textureHandleIndex
) : GameObject(objectID, name) {
    this->bindTexture(textureHandleIndex);
}

RenderableObjectBase& RenderableObjectBase::bindTexture(
    u32 textureHandleIndex
) {
    this->textureHandle = Program::getResourceManager().getTextureHandle(textureHandleIndex);
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
    const u32 textureHandleIndex
) : RenderableObjectBase(objectID, textureHandleIndex) {
    this->setTexturePortionOriginal();
}

RenderableObject::RenderableObject(
    const u32 objectID,
    const char* name,
    const u32 textureHandleIndex
) : RenderableObjectBase(objectID, name, textureHandleIndex) {
    this->setTexturePortionOriginal();
}

RenderableObject& RenderableObject::setTexturePortion(const SDL_Rect& r) {
    this->texturePortion = r;
    return *this;
}

RenderableObject& RenderableObject::setTexturePortionOriginal() {
    this->texturePortion.x = this->texturePortion.y = 0;
    this->texturePortion.h = (int)this->textureHandle.originalSize.height;
    this->texturePortion.w = (int)this->textureHandle.originalSize.width;
    return *this;
}

RenderableObject& RenderableObject::setTargetPortion(const SDL_Rect& r) {
    this->targetPortion = r;
    return *this;
}

RenderableObject& RenderableObject::scaleX(float scale) {
    this->targetPortion.w = (int)(scale * (float)this->textureHandle.originalSize.width);
    return *this;
}

RenderableObject& RenderableObject::scaleY(float scale) {
    this->targetPortion.h = (int)(scale * (float)this->textureHandle.originalSize.height);
    return *this;
}

RenderableObject& RenderableObject::scale(float scale) {
    this->targetPortion.w = (int)(scale * (float)this->textureHandle.originalSize.width);
    this->targetPortion.h = (int)(scale * (float)this->textureHandle.originalSize.height);
    return *this;
}

RenderableObject& RenderableObject::scale(float scaleX, float scaleY) {
    this->targetPortion.w = (int)(scaleX * (float)this->textureHandle.originalSize.width);
    this->targetPortion.h = (int)(scaleY * (float)this->textureHandle.originalSize.height);
    return *this;
}

void RenderableObject::rotate(double degrees) {
    if(this->angle > 360.0) {
        this->angle -= 360.0;
    }
    this->angle += degrees;
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