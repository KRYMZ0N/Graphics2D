#ifndef GRAPHICS_LIB_H
#define GRAPHICS_LIB_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>

// Forward declaration so Sprite classes know the Engine exists
class GraphicsEngine;

// 1. Sprite Class for single images
class Sprite {
friend class GraphicsEngine; // Allows Engine to access private texture
public:
    Sprite(GraphicsEngine& engine, const std::string& filePath);
    ~Sprite();
    bool isLoaded() const { return texture != nullptr; }

private:
    SDL_Texture* texture = nullptr;
    int width = 0;
    int height = 0;
};

// 2. SpriteSheet Class for grid templates
class SpriteSheet {
friend class GraphicsEngine;
public:
    SpriteSheet(GraphicsEngine& engine, const std::string& filePath, int frameW, int frameH);
    ~SpriteSheet();
    bool isLoaded() const { return texture != nullptr; }

private:
    SDL_Texture* texture = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
};

// 3. Core Graphics Engine
class GraphicsEngine {
public:
    GraphicsEngine(const std::string& title, int width, int height);
    ~GraphicsEngine();

    void clear(uint8_t r, uint8_t g, uint8_t b);
    void present();
    
    // Draw a single picture
    void drawSprite(const Sprite& sprite, int x, int y, int scale = 1);
    
    // Draw a template frame from a sheet
    void drawSpriteFrame(const SpriteSheet& sheet, int frameX, int frameY, int screenX, int screenY, int scale = 1);

    // Helper for loading textures inside Sprite classes
    SDL_Texture* loadTexture(const std::string& filePath, int& outW, int& outH);

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

#endif