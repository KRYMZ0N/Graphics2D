#ifndef GRAPHICS_LIB_H
#define GRAPHICS_LIB_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

class GraphicsEngine;

class Sprite {
friend class GraphicsEngine;
public:
    Sprite(GraphicsEngine& engine, const std::string& filePath);
    ~Sprite();

    bool isLoaded() const { return texture != nullptr; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    SDL_Texture* texture = nullptr;
    int width = 0;
    int height = 0;
};

class SpriteSheet {
friend class GraphicsEngine;
public:
    SpriteSheet(GraphicsEngine& engine, const std::string& filePath, int frameW, int frameH);
    ~SpriteSheet();

    bool isLoaded() const { return texture != nullptr; }
    int getFrameWidth() const { return frameWidth; }
    int getFrameHeight() const { return frameHeight; }

private:
    SDL_Texture* texture = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
};

class GraphicsEngine {
public:
    GraphicsEngine(const std::string& title, int width, int height);
    ~GraphicsEngine();

    void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void present();

    void setCamera(int x, int y);

    // Draw a single image in world space with optional horizontal flip.
    void drawSprite(const Sprite& sprite, int x, int y, int scale = 1, bool flipX = false);

    // Draw one frame from a sprite sheet.
    void drawSpriteFrame(const SpriteSheet& sheet, int frameX, int frameY, int screenX, int screenY, int scale = 1, bool flipX = false);

    // Simple geometric primitives for UI and debug overlays.
    void drawRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void fillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    int getWidth() const { return windowWidth; }
    int getHeight() const { return windowHeight; }
    SDL_Renderer* getRenderer() const { return renderer; }

    // Helper for loading textures inside Sprite classes.
    SDL_Texture* loadTexture(const std::string& filePath, int& outW, int& outH);

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int windowWidth = 0;
    int windowHeight = 0;
    int cameraX = 0;
    int cameraY = 0;
};

#endif