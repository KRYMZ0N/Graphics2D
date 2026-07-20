#include "GraphicsLib.h"
#include <SDL2/SDL.h>       // Include SDL here, completely hidden from the game!
#include <SDL2/SDL_image.h>

GraphicsEngine::GraphicsEngine(const std::string& title, int width, int height)
    : windowWidth(width), windowHeight(height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
    }

    int imgFlags = IMG_Init(IMG_INIT_PNG);
    if ((imgFlags & IMG_INIT_PNG) == 0) {
        SDL_Quit();
        throw std::runtime_error(std::string("Failed to initialize SDL_image: ") + IMG_GetError());
    }

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN);

    if (!window) {
        IMG_Quit();
        SDL_Quit();
        throw std::runtime_error(std::string("Failed to create SDL window: ") + SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        throw std::runtime_error(std::string("Failed to create SDL renderer: ") + SDL_GetError());
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

GraphicsEngine::~GraphicsEngine() {
    // Cleanup any lingering textures before destroying the renderer
    for (auto& pair : textureCache) {
        SDL_DestroyTexture(pair.second.texture);
    }
    textureCache.clear();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* GraphicsEngine::acquireTexture(const std::string& filePath, int& outW, int& outH) {
    auto it = textureCache.find(filePath);
    if (it != textureCache.end()) {
        it->second.refCount++;
        SDL_QueryTexture(it->second.texture, nullptr, nullptr, &outW, &outH);
        return it->second.texture;
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture '" << filePath << "': " << IMG_GetError() << std::endl;
        outW = 0;
        outH = 0;
        return nullptr;
    }

    SDL_SetTextureScaleMode(texture, SDL_ScaleModeNearest);
    SDL_QueryTexture(texture, nullptr, nullptr, &outW, &outH);

    textureCache[filePath] = { texture, 1 };
    return texture;
}

bool GraphicsEngine::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false; // Tell the game loop to stop running
        }
    }
    return true; // Keep running
}

void GraphicsEngine::releaseTexture(const std::string& filePath) {
    auto it = textureCache.find(filePath);
    if (it != textureCache.end()) {
        it->second.refCount--;
        if (it->second.refCount <= 0) {
            SDL_DestroyTexture(it->second.texture);
            textureCache.erase(it);
        }
    }
}

void GraphicsEngine::clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}

void GraphicsEngine::present() {
    SDL_RenderPresent(renderer);
}

void GraphicsEngine::setCamera(int x, int y) {
    cameraX = x;
    cameraY = y;
}

void GraphicsEngine::drawSprite(const Sprite& sprite, int x, int y, int scale, bool flipX) {
    if (!sprite.texture) return;

    SDL_Rect destRect = {
        x - cameraX,
        y - cameraY,
        sprite.width * scale,
        sprite.height * scale
    };

    SDL_RenderCopyEx(renderer, sprite.texture, nullptr, &destRect, 0.0, nullptr, flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void GraphicsEngine::drawSpriteFrame(const SpriteSheet& sheet, int frameX, int frameY, int screenX, int screenY, int scale, bool flipX) {
    if (!sheet.texture) return;

    SDL_Rect srcRect = {
        frameX * sheet.frameWidth,
        frameY * sheet.frameHeight,
        sheet.frameWidth,
        sheet.frameHeight
    };

    SDL_Rect destRect = {
        screenX - cameraX,
        screenY - cameraY,
        sheet.frameWidth * scale,
        sheet.frameHeight * scale
    };

    SDL_RenderCopyEx(renderer, sheet.texture, &srcRect, &destRect, 0.0, nullptr, flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void GraphicsEngine::drawRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool isWorldSpace) {
    int renderX = isWorldSpace ? (x - cameraX) : x;
    int renderY = isWorldSpace ? (y - cameraY) : y;
    SDL_Rect rect = { renderX, renderY, w, h };
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawRect(renderer, &rect);
}

void GraphicsEngine::fillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool isWorldSpace) {
    int renderX = isWorldSpace ? (x - cameraX) : x;
    int renderY = isWorldSpace ? (y - cameraY) : y;
    SDL_Rect rect = { renderX, renderY, w, h };
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void GraphicsEngine::drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool isWorldSpace) {
    int rX1 = isWorldSpace ? (x1 - cameraX) : x1;
    int rY1 = isWorldSpace ? (y1 - cameraY) : y1;
    int rX2 = isWorldSpace ? (x2 - cameraX) : x2;
    int rY2 = isWorldSpace ? (y2 - cameraY) : y2;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawLine(renderer, rX1, rY1, rX2, rY2);
}

Sprite::Sprite(GraphicsEngine& engine, const std::string& filePath) 
    : engine(&engine), filePath(filePath) {
    texture = engine.acquireTexture(filePath, width, height);
}

Sprite::~Sprite() {
    if (engine && !filePath.empty()) {
        engine->releaseTexture(filePath);
    }
}

SpriteSheet::SpriteSheet(GraphicsEngine& engine, const std::string& filePath, int frameW, int frameH)
    : engine(&engine), filePath(filePath), frameWidth(frameW), frameHeight(frameH) {
    int totalW = 0;
    int totalH = 0;
    texture = engine.acquireTexture(filePath, totalW, totalH);
}

SpriteSheet::~SpriteSheet() {
    if (engine && !filePath.empty()) {
        engine->releaseTexture(filePath);
    }
}