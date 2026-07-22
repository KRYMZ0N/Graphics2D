#include "GraphicsLib.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Sprite::Sprite(GraphicsEngine& eng, const std::string& path) 
    : engine(&eng), filePath(path) {
    texture = engine->acquireTexture(filePath, width, height);
}

Sprite::~Sprite() {
    if (engine && !filePath.empty()) {
        engine->releaseTexture(filePath);
    }
}

SpriteSheet::SpriteSheet(GraphicsEngine& eng, const std::string& path, int frameW, int frameH)
    : engine(&eng), filePath(path), frameWidth(frameW), frameHeight(frameH) {
    int totalW = 0;
    int totalH = 0;
    texture = engine->acquireTexture(filePath, totalW, totalH);
}

SpriteSheet::~SpriteSheet() {
    if (engine && !filePath.empty()) {
        engine->releaseTexture(filePath);
    }
}