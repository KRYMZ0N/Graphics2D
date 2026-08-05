#include "GraphicsLib.h"
#include <SDL2/SDL.h>       // Include SDL here, completely hidden from the game!
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> // Add this near your other SDL includes

struct GraphicsEngine::Impl {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
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
        std::cerr << "\n[CRITICAL SDL ERROR] SDL_Init failed: " << SDL_GetError() << "\n";
        exit(1); // Force print and exit safely
    }

    int imgFlags = IMG_Init(IMG_INIT_PNG);
    if ((imgFlags & IMG_INIT_PNG) == 0) {
        std::cerr << "\n[CRITICAL IMG ERROR] IMG_Init failed: " << IMG_GetError() << "\n";
        exit(1);
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "\n[CRITICAL TTF ERROR] TTF_Init failed: " << TTF_GetError() << "\n";
        exit(1);
    }

    // Load your font (You will need a .ttf file in your assets folder!)
    pImpl->font = TTF_OpenFont("assets/font.ttf", 24); // 24 is the font size
    if (!pImpl->font) {
        std::cout << "[WARNING] Failed to load font.ttf! Text will not render.\n";
    }


    pImpl->window = SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!pImpl->window) {
        std::cerr << "\n[CRITICAL WINDOW ERROR] SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        exit(1);
    }

    pImpl->renderer = SDL_CreateRenderer(pImpl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!pImpl->renderer) {
        std::cout << "[DEBUG] Accelerated renderer failed, trying software fallback...\n";
        pImpl->renderer = SDL_CreateRenderer(pImpl->window, -1, SDL_RENDERER_SOFTWARE);
    }
    
    if (!pImpl->renderer) {
        std::cerr << "\n[CRITICAL RENDERER ERROR] SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        exit(1);
    }

    // --- 2. LOGICAL SCALING & NEAREST-NEIGHBOR PIXEL ART FILTERING ---
    // Maintains 800x600 logical internal resolution regardless of window dimensions
    SDL_RenderSetLogicalSize(pImpl->renderer, width, height);
    
    // Nearest-neighbor scaling ensures pixel graphics stay crisp when scaled up
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
}

// 3. Clean up pImpl in the destructor
GraphicsEngine::~GraphicsEngine() {
    for (auto& pair : pImpl->textureCache) {
        SDL_DestroyTexture(pair.second.texture);
    }
    pImpl->textureCache.clear();

    if (pImpl->renderer) SDL_DestroyRenderer(pImpl->renderer);
    if (pImpl->window) SDL_DestroyWindow(pImpl->window);

    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
    }
    TTF_Quit();
    
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

// Returns true to keep running, false if the X button is clicked
bool GraphicsEngine::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
    }
    return true;
}

// Frame rate delay
void GraphicsEngine::delay(int ms) {
    SDL_Delay(ms);
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

void GraphicsEngine::getCamera(int& camX, int& camY) const {
    camX = pImpl->cameraX;
    camY = pImpl->cameraY;
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

void GraphicsEngine::drawSprite(const Sprite& sprite, int x, int y, int scale, bool flipX) {
    if (!sprite.texture) return;
    SDL_Rect destRect = { x - pImpl->cameraX, y - pImpl->cameraY, sprite.width * scale, sprite.height * scale };
    SDL_RenderCopyEx(pImpl->renderer, sprite.texture, nullptr, &destRect, 0.0, nullptr, flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void GraphicsEngine::fillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool isWorldSpace) {
    int renderX = isWorldSpace ? (x - pImpl->cameraX) : x;
    int renderY = isWorldSpace ? (y - pImpl->cameraY) : y;
    SDL_Rect rect = { renderX, renderY, w, h };
    SDL_SetRenderDrawColor(pImpl->renderer, r, g, b, a);
    SDL_RenderFillRect(pImpl->renderer, &rect);
}

void GraphicsEngine::drawRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool isWorldSpace) {
    int renderX = isWorldSpace ? (x - pImpl->cameraX) : x;
    int renderY = isWorldSpace ? (y - pImpl->cameraY) : y;
    SDL_Rect rect = { renderX, renderY, w, h };
    SDL_SetRenderDrawColor(pImpl->renderer, r, g, b, a);
    SDL_RenderDrawRect(pImpl->renderer, &rect);
}

void GraphicsEngine::drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool isWorldSpace) {
    int rx1 = isWorldSpace ? (x1 - pImpl->cameraX) : x1;
    int ry1 = isWorldSpace ? (y1 - pImpl->cameraY) : y1;
    int rx2 = isWorldSpace ? (x2 - pImpl->cameraX) : x2;
    int ry2 = isWorldSpace ? (y2 - pImpl->cameraY) : y2;
    SDL_SetRenderDrawColor(pImpl->renderer, r, g, b, a);
    SDL_RenderDrawLine(pImpl->renderer, rx1, ry1, rx2, ry2);
}

void GraphicsEngine::drawSpriteFrame(const SpriteSheet& sheet, int frameX, int frameY, int screenX, int screenY, int scale, bool flipX) {
    if (!sheet.texture) return;

    // 1. define the source rectangle on the sprite sheet grid
    SDL_Rect srcRect = {
        frameX * sheet.frameWidth,
        frameY * sheet.frameHeight,
        sheet.frameWidth,
        sheet.frameHeight
    };

    // 2. Define the destination rectangle on the screen
    SDL_Rect destRect = {
        screenX - pImpl->cameraX,
        screenY - pImpl->cameraY,
        sheet.frameWidth * scale,
        sheet.frameHeight * scale
    };

    // 3. Render the specific frame slice with optional horizontal flipping
    SDL_RenderCopyEx(pImpl->renderer, sheet.texture, &srcRect, &destRect, 0.0, nullptr, flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void GraphicsEngine::drawText(const std::string& text, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (!pImpl->font || text.empty()) return;

    SDL_Color color = { r, g, b, 255 };
    
    // 1. Create a pixel surface from the string
    SDL_Surface* surface = TTF_RenderText_Solid(pImpl->font, text.c_str(), color);
    if (!surface) return;

    // 2. Convert to a hardware texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(pImpl->renderer, surface);
    if (texture) {
        // 3. Define where it goes and draw it
        SDL_Rect destRect = { x, y, surface->w, surface->h };
        SDL_RenderCopy(pImpl->renderer, texture, nullptr, &destRect);
        
        // 4. Clean up the texture
        SDL_DestroyTexture(texture);
    }
    
    // 5. Clean up the surface
    SDL_FreeSurface(surface);
}

void GraphicsEngine::drawSpriteRotated(const Sprite& sprite, int x, int y, int scale, double angle, bool flipX, bool isWorldSpace) {
    if (!sprite.texture) return;

    // Adjust for camera offset if in world space
    int renderX = isWorldSpace ? (x - pImpl->cameraX) : x;
    int renderY = isWorldSpace ? (y - pImpl->cameraY) : y;

    SDL_Rect destRect = { renderX, renderY, sprite.width * scale, sprite.height * scale };

    // Set rotation center (pivot point at the bottom center of the sprite, perfect for handles)
    SDL_Point center = { (sprite.width * scale) / 2, sprite.height * scale };

    SDL_RendererFlip flip = flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    // Render rotated using SDL_RenderCopyEx
    SDL_RenderCopyEx(pImpl->renderer, sprite.texture, nullptr, &destRect, angle, &center, flip);
}