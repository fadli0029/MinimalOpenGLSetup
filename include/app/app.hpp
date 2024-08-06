#ifndef APP_HPP
#define APP_HPP

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <random>
#include <string>

#include "cameras/arcball_camera.hpp"
#include "cameras/fps_camera.hpp"
#include "input_handlers/arcball_input_handler.hpp"
#include "input_handlers/fps_input_handler.hpp"
#include "shader/shader.hpp"

// Test bullet3 includes
#include "btBulletDynamicsCommon.h"

class App {
  public:
    enum class CameraType { FPS, Arcball };

    App(int width, int height, const std::string &title,
        const char *vertexShader, const char *fragmentShader);
    ~App();

    bool Initialize();
    void Run();

  private:
    void GetOpenGLVersionInfo();
    void InitOpenGL();
    void ProcessInput();
    void Update();
    void Render();
    void SwitchCamera();
    void UpdateViewport(const int &width, const int &height);
    void CleanUp();

    int screenWidth;
    int screenHeight;
    std::string windowTitle;

    const char *vertexShaderPath;
    const char *fragmentShaderPath;

    float deltaTime;
    float lastFrame;

    SDL_Window *window;
    SDL_GLContext context;
    bool quit;

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    unsigned int planeVAO, planeVBO, planeEBO;
    Shader *shader;

    FPSCamera fpsCamera;
    InputHandler *fpsInputHandler;
    ArcballCamera arcballCamera;
    InputHandler *arcballInputHandler;

    CameraType activeCameraType;
    InputHandler *activeInputHandler;
};

#endif // APP_HPP
