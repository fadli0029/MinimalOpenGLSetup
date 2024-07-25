#ifndef BASE_HPP
#define BASE_HPP

#include <SDL2/SDL.h>

// Base input handler class
class InputHandler {
public:
    virtual ~InputHandler() = default;
    virtual void HandleKeyboardInput(const SDL_Event& e, float deltaTime) = 0;
    virtual void HandleMouseMotion(const SDL_Event& e) = 0;
    virtual void HandleMouseWheel(const SDL_Event& e) = 0;
};

#endif // BASE_HPP
