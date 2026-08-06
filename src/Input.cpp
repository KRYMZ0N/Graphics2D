#include "GraphicsLib.h"
#include <SDL2/SDL.h>
#include <cctype> // For std::toupper

bool GraphicsEngine::isKeyDown(char key) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    
    // Support ASCII uppercase/lowercase dynamically ('A'-'Z')
    char upperKey = static_cast<char>(std::toupper(static_cast<unsigned char>(key)));
    if (upperKey >= 'A' && upperKey <= 'Z') {
        return state[SDL_SCANCODE_A + (upperKey - 'A')] != 0;
    }
    
    // Support numbers ('0'-'9')
    if (upperKey >= '0' && upperKey <= '9') {
        return state[SDL_SCANCODE_0 + (upperKey - '0')] != 0;
    }

    // Special keys
    if (key == ' ')    return state[SDL_SCANCODE_SPACE] != 0;
    if (key == '\033') return state[SDL_SCANCODE_ESCAPE] != 0; // Escape key
    
    return false;
}

bool GraphicsEngine::isMouseButtonDown(int button) {
    int x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);

    if (button == 1) return (buttons & SDL_BUTTON_LMASK) != 0; // Left Click (MB1)
    if (button == 2) return (buttons & SDL_BUTTON_MMASK) != 0; // Middle Click
    if (button == 3) return (buttons & SDL_BUTTON_RMASK) != 0; // Right Click (MB2)

    return false;
}

void GraphicsEngine::getMousePosition(int& mouseX, int& mouseY) const {
    SDL_GetMouseState(&mouseX, &mouseY);
}