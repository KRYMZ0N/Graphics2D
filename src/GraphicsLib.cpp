#include "GraphicsLib.h"

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
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* GraphicsEngine::loadTexture(const std::string& filePath, int& outW, int& outH) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture '" << filePath << "': " << IMG_GetError() << std::endl;
        outW = 0;
        outH = 0;
        return nullptr;
    }

    SDL_SetTextureScaleMode(texture, SDL_ScaleModeNearest);
    SDL_QueryTexture(texture, nullptr, nullptr, &outW, &outH);
    return texture;
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
    if (!sprite.texture) {
        return;
    }

    SDL_Rect destRect = {
        x - cameraX,
        y - cameraY,
        sprite.width * scale,
        sprite.height * scale
    };

    SDL_RenderCopyEx(
        renderer,
        sprite.texture,
        nullptr,
        &destRect,
        0.0,
        nullptr,
        flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void GraphicsEngine::drawSpriteFrame(const SpriteSheet& sheet, int frameX, int frameY, int screenX, int screenY, int scale, bool flipX) {
    if (!sheet.texture) {
        return;
    }

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

    SDL_RenderCopyEx(
        renderer,
        sheet.texture,
        &srcRect,
        &destRect,
        0.0,
        nullptr,
        flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void GraphicsEngine::drawRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_Rect rect = { x - cameraX, y - cameraY, w, h };
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawRect(renderer, &rect);
}

void GraphicsEngine::fillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_Rect rect = { x - cameraX, y - cameraY, w, h };
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void GraphicsEngine::drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawLine(renderer, x1 - cameraX, y1 - cameraY, x2 - cameraX, y2 - cameraY);
}

Sprite::Sprite(GraphicsEngine& engine, const std::string& filePath) {
    texture = engine.loadTexture(filePath, width, height);
}

Sprite::~Sprite() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

SpriteSheet::SpriteSheet(GraphicsEngine& engine, const std::string& filePath, int frameW, int frameH)
    : frameWidth(frameW), frameHeight(frameH) {
    int totalW = 0;
    int totalH = 0;
    texture = engine.loadTexture(filePath, totalW, totalH);
}

SpriteSheet::~SpriteSheet() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}