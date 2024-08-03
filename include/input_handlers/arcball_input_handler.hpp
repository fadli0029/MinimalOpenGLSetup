#ifndef ARCBALL_INPUT_HANDLER_HPP
#define ARCBALL_INPUT_HANDLER_HPP

#include "base.hpp"
#include "arcball_camera.hpp"

class ArcballInputHandler : public InputHandler {
  public:
    ArcballInputHandler(ArcballCamera &arcballCamera)
        : arcballCamera(arcballCamera) {}

    void HandleInput(const SDL_Event &e, float deltaTime) override {
        if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
            HandleMouseClick(e);
        } else if (e.type == SDL_MOUSEMOTION) {
            HandleMouseMotion(e);
        } else if (e.type == SDL_MOUSEWHEEL) {
            HandleMouseWheel(e);
        }
    }

    void HandleKeyboardInput(const SDL_Event &e, float deltaTime) override {}

    void HandleMouseMotion(const SDL_Event &e) override {
        if (leftButtonPressed) {
            float xoffset = e.motion.xrel;
            float yoffset = e.motion.yrel;

            if (SDL_GetModState() & KMOD_CTRL) {
                arcballCamera.ProcessMouseRotation(-xoffset, -yoffset);
            } else {
                arcballCamera.ProcessMousePan(-xoffset * 0.01f,
                                              yoffset * 0.01f);
            }
        }
    }

    virtual void HandleMouseClick(const SDL_Event &e) override {
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                leftButtonPressed = true;
            }
        } else {
            if (e.button.button == SDL_BUTTON_LEFT) {
                leftButtonPressed = false;
            }
        }
    }

    void HandleMouseWheel(const SDL_Event &e) override {
        arcballCamera.ProcessMouseScroll(e.wheel.y);
    }

  private:
    ArcballCamera &arcballCamera;
    bool leftButtonPressed = false;
};

#endif // ARCBALL_INPUT_HANDLER_HPP
