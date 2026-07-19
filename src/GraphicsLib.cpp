#include "GraphicsLib.h"

// === ENGINE IMPLEMENTATION ===
GraphicsEngine::GraphicsEngine(const std::string& title, int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG); // Enable PNG loading

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    
    // Create hardware-accelerated renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

GraphicsEngine::~GraphicsEngine() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* GraphicsEngine::loadTexture(const std::string& filePath, int& outW, int& outH) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
    if (!texture) {
        std::cerr << "Failed to load pixel art: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    
    // CRITICAL FOR PIXEL ART: Set texture scaling mode to Nearest Neighbor (No Blur!)
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeNearest);
    
    SDL_QueryTexture(texture, nullptr, nullptr, &outW, &outH);
    return texture;
}

void GraphicsEngine::clear(uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
}

void GraphicsEngine::present() {
    SDL_RenderPresent(renderer);
}

void GraphicsEngine::drawSprite(const Sprite& sprite, int x, int y, int scale) {
    if (!sprite.texture) return;
    SDL_Rect destRect = { x, y, sprite.width * scale, sprite.height * scale };
    SDL_RenderCopy(renderer, sprite.texture, nullptr, &destRect);
}

void GraphicsEngine::drawSpriteFrame(const SpriteSheet& sheet, int frameX, int frameY, int screenX, int screenY, int scale) {
    if (!sheet.texture) return;

    // Source rectangle: Where on the template image to cut out
    SDL_Rect srcRect = { 
        frameX * sheet.frameWidth, 
        frameY * sheet.frameHeight, 
        sheet.frameWidth, 
        sheet.frameHeight 
    };

    // Destination rectangle: Where on the screen to draw it (scaled up)
    SDL_Rect destRect = { 
        screenX, 
        screenY, 
        sheet.frameWidth * scale, 
        sheet.frameHeight * scale 
    };

    SDL_RenderCopy(renderer, sheet.texture, &srcRect, &destRect);
}

// === SPRITE IMPLEMENTATIONS ===
Sprite::Sprite(GraphicsEngine& engine, const std::string& filePath) {
    texture = engine.loadTexture(filePath, width, height);
}
Sprite::~Sprite() { if (texture) SDL_DestroyTexture(texture); }

SpriteSheet::SpriteSheet(GraphicsEngine& engine, const std::string& filePath, int frameW, int frameH) 
    : frameWidth(frameW), frameHeight(frameH) {
    int totalW, totalH;
    texture = engine.loadTexture(filePath, totalW, totalH);
}
SpriteSheet::~SpriteSheet() { if (texture) SDL_DestroyTexture(texture); }