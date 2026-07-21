#include "GraphicsLib.h"
#include <SDL2/SDL.h>       // Include SDL here, completely hidden from the game!
#include <SDL2/SDL_image.h>

struct GraphicsEngine::Impl {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int windowWidth = 0;
    int windowHeight = 0;
    int cameraX = 0;
    int cameraY = 0;

    struct CachedTexture {
        SDL_Texture* texture;
        int refCount;
    };
    std::unordered_map<std::string, CachedTexture> textureCache;
};

GraphicsEngine::GraphicsEngine(const std::string& title, int width, int height) {
    pImpl = new Impl();
    pImpl->windowWidth = width;
    pImpl->windowHeight = height;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
    }

    int imgFlags = IMG_Init(IMG_INIT_PNG);
    if ((imgFlags & IMG_INIT_PNG) == 0) {
        SDL_Quit();
        throw std::runtime_error(std::string("Failed to initialize SDL_image: ") + IMG_GetError());
    }

    pImpl->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!pImpl->window) {
        std::string err = SDL_GetError();
        IMG_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL window: " + err);
    }

    pImpl->renderer = SDL_CreateRenderer(pImpl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!pImpl->renderer) {
        pImpl->renderer = SDL_CreateRenderer(pImpl->window, -1, SDL_RENDERER_SOFTWARE);
    }
    
    if (!pImpl->renderer) {
        SDL_DestroyWindow(pImpl->window);
        IMG_Quit();
        SDL_Quit();
        throw std::runtime_error(std::string("Failed to create SDL renderer: ") + SDL_GetError());
    }
}

// 3. Clean up pImpl in the destructor
GraphicsEngine::~GraphicsEngine() {
    for (auto& pair : pImpl->textureCache) {
        SDL_DestroyTexture(pair.second.texture);
    }
    pImpl->textureCache.clear();

    if (pImpl->renderer) SDL_DestroyRenderer(pImpl->renderer);
    if (pImpl->window) SDL_DestroyWindow(pImpl->window);
    
    IMG_Quit();
    SDL_Quit();

    delete pImpl; // Free the hidden struct memory
}

bool GraphicsEngine::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return false;
    }
    return true;
}

void GraphicsEngine::clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(pImpl->renderer, r, g, b, a);
    SDL_RenderClear(pImpl->renderer);
}

void GraphicsEngine::present() {
    SDL_RenderPresent(pImpl->renderer);
}

void GraphicsEngine::setCamera(int x, int y) {
    pImpl->cameraX = x;
    pImpl->cameraY = y;
}

int GraphicsEngine::getWidth() const { return pImpl->windowWidth; }
int GraphicsEngine::getHeight() const { return pImpl->windowHeight; }
SDL_Renderer* GraphicsEngine::getRenderer() const { return pImpl->renderer; }

SDL_Texture* GraphicsEngine::acquireTexture(const std::string& filePath, int& outW, int& outH) {
    auto it = pImpl->textureCache.find(filePath);
    if (it != pImpl->textureCache.end()) {
        it->second.refCount++;
        SDL_QueryTexture(it->second.texture, nullptr, nullptr, &outW, &outH);
        return it->second.texture;
    }

    SDL_Texture* texture = IMG_LoadTexture(pImpl->renderer, filePath.c_str());
    if (!texture) return nullptr;

    SDL_SetTextureScaleMode(texture, SDL_ScaleModeNearest);
    SDL_QueryTexture(texture, nullptr, nullptr, &outW, &outH);

    pImpl->textureCache[filePath] = { texture, 1 };
    return texture;
}

void GraphicsEngine::releaseTexture(const std::string& filePath) {
    auto it = pImpl->textureCache.find(filePath);
    if (it != pImpl->textureCache.end()) {
        it->second.refCount--;
        if (it->second.refCount <= 0) {
            SDL_DestroyTexture(it->second.texture);
            pImpl->textureCache.erase(it);
        }
    }
}

// (Update your drawSprite, fillRectangle, etc. to use pImpl->renderer and pImpl->cameraX/Y as well)
void GraphicsEngine::drawSprite(const Sprite& sprite, int x, int y, int scale, bool flipX) {
    if (!sprite.texture) return;
    SDL_Rect destRect = { x - pImpl->cameraX, y - pImpl->cameraY, sprite.width * scale, sprite.height * scale };
    SDL_RenderCopyEx(pImpl->renderer, sprite.texture, nullptr, &destRect, 0.0, nullptr, flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

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