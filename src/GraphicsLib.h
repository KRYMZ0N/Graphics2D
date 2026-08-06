#ifndef GRAPHICS_LIB_H
#define GRAPHICS_LIB_H

// Windows DLL Import/Export Macro Setup
#if defined(_WIN32) || defined(_WIN64)
    #ifdef PIXEL_GRAPHICS_EXPORTS
        #define PIXEL_API __declspec(dllexport)
    #else
        #define PIXEL_API __declspec(dllimport)
    #endif
#else
    #define PIXEL_API
#endif

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class GraphicsEngine;

class PIXEL_API Sprite {
friend class GraphicsEngine;
public:
    Sprite(GraphicsEngine& engine, const std::string& filePath);
    ~Sprite();

    bool isLoaded() const { return texture != nullptr; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    GraphicsEngine* engine = nullptr;
    std::string filePath;
    SDL_Texture* texture = nullptr;
    int width = 0;
    int height = 0;
};

class PIXEL_API SpriteSheet {
friend class GraphicsEngine;
public:
    SpriteSheet(GraphicsEngine& engine, const std::string& filePath, int frameW, int frameH);
    ~SpriteSheet();

    bool isLoaded() const { return texture != nullptr; }
    int getFrameWidth() const { return frameWidth; }
    int getFrameHeight() const { return frameHeight; }

private:
    GraphicsEngine* engine = nullptr;
    std::string filePath;
    SDL_Texture* texture = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
};

class PIXEL_API GraphicsEngine {
friend class Sprite;
friend class SpriteSheet;
public:
    GraphicsEngine(const std::string& title, int width, int height);
    ~GraphicsEngine();
    bool processInput();

    bool processEvents(); // Returns false if the window is closed
    bool isKeyDown(char key);

    // New Mouse Input Functions
    bool isMouseButtonDown(int button); // 1 = Left (MB1), 2 = Middle, 3 = Right (MB2)
    void getMousePosition(int& mouseX, int& mouseY) const;
    
    void delay(int ms);

    void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void present();
    void setCamera(int x, int y);
    void getCamera(int& camX, int& camY) const;

    void drawSprite(const Sprite& sprite, int x, int y, int scale = 1, bool flipX = false);
    void drawSpriteFrame(const SpriteSheet& sheet, int frameX, int frameY, int screenX, int screenY, int scale = 1, bool flipX = false);
    // Add to GraphicsEngine class declaration in GraphicsLib.h:
    void drawSpriteRotated(const Sprite& sprite, int x, int y, int scale, double angle, 
                           int pivotX = -1, int pivotY = -1, bool flipX = false, bool isWorldSpace = true);

    void drawRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool isWorldSpace = true);
    void fillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool isWorldSpace = true);
    void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool isWorldSpace = true);
    void drawText(const std::string& text, int x, int y, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255);
    
    void setSpriteTint(const Sprite& sprite, uint8_t r, uint8_t g, uint8_t b);
    void setSpriteAlpha(const Sprite& sprite, uint8_t alpha);
    void resetSpriteTint(const Sprite& sprite);

    void getTextDimensions(const std::string& text, int& outW, int& outH);

    int getWidth() const;
    int getHeight() const;
    SDL_Renderer* getRenderer() const;

private:
    SDL_Texture* acquireTexture(const std::string& filePath, int& outW, int& outH);
    void releaseTexture(const std::string& filePath);

    // PImpl Idiom: Hide all memory layout details from the executable!
    struct Impl;
    Impl* pImpl;
};

#endif