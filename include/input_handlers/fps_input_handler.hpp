#ifndef FPS_INPUT_HANDLER_HPP
#define FPS_INPUT_HANDLER_HPP

#include "base.hpp"
#include "fps_camera.hpp"

// FPS input handler class
class FPSInputHandler : public InputHandler {
  public:
    FPSInputHandler(FPSCamera &fpsCamera) : fpsCamera(fpsCamera) {}

    void HandleInput(const SDL_Event &e, float deltaTime) override {
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            HandleKeyboardInput(e, deltaTime);
        } else if (e.type == SDL_MOUSEMOTION) {
            HandleMouseMotion(e);
        } else if (e.type == SDL_MOUSEWHEEL) {
            HandleMouseWheel(e);
        }
    }

    void HandleKeyboardInput(const SDL_Event &e, float deltaTime) override {
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_w:
                fpsCamera.ProcessKeyboard(FORWARD, deltaTime);
                break;
            case SDLK_s:
                fpsCamera.ProcessKeyboard(BACKWARD, deltaTime);
                break;
            case SDLK_a:
                fpsCamera.ProcessKeyboard(LEFT, deltaTime);
                break;
            case SDLK_d:
                fpsCamera.ProcessKeyboard(RIGHT, deltaTime);
                break;
            case SDLK_ESCAPE:
                SDL_Event quitEvent;
                quitEvent.type = SDL_QUIT;
                SDL_PushEvent(&quitEvent);
                break;
            }
        }
    }
    void HandleMouseMotion(const SDL_Event &e) override {
        fpsCamera.ProcessMouseMovement(e.motion.xrel, -e.motion.yrel);
    }

    void HandleMouseClick(const SDL_Event &e) override {}

    void HandleMouseWheel(const SDL_Event &e) override {
        fpsCamera.ProcessMouseScroll(e.wheel.y);
    }

  private:
    FPSCamera &fpsCamera;
};

#endif // FPS_INPUT_HANDLER_HPP
