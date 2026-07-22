#include "GraphicsLib.h"
#include <SDL2/SDL.h>

bool GraphicsEngine::isKeyDown(char key) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (key == 'W' || key == 'w') return state[SDL_SCANCODE_W];
    if (key == 'A' || key == 'a') return state[SDL_SCANCODE_A];
    if (key == 'S' || key == 's') return state[SDL_SCANCODE_S];
    if (key == 'D' || key == 'd') return state[SDL_SCANCODE_D];
    if (key == 'E' || key == 'e') return state[SDL_SCANCODE_E];
    if (key == 'Q' || key == 'q') return state[SDL_SCANCODE_Q];
    if (key == 'F' || key == 'f') return state[SDL_SCANCODE_F];
    if (key == 'R' || key == 'r') return state[SDL_SCANCODE_R];
    if (key == 'L' || key == 'l') return state[SDL_SCANCODE_L];
    if (key == ' ')               return state[SDL_SCANCODE_SPACE];
    return false;
}

bool GraphicsEngine::isMouseButtonDown(int button) {
    int x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);
    if (button == 1) return (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;   // MB1
    if (button == 2) return (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0; // Middle
    if (button == 3) return (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;  // MB2
    return false;
}

void GraphicsEngine::getMousePosition(int& mouseX, int& mouseY) const {
    SDL_GetMouseState(&mouseX, &mouseY);
}