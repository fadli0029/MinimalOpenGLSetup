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

// Bullet Physics includes
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

    // Bullet Physics
    void InitPhysics();
    void ShutdownPhysics();
    void CreatePhysicsObjects();

    int screenWidth_, screenHeight_;
    std::string windowTitle_;

    const char *vertexShaderPath_, *fragmentShaderPath_;

    float deltaTime_, lastFrame_;

    SDL_Window *window_;
    SDL_GLContext context_;
    bool quit_;

    unsigned int cubeVAO_, cubeVBO_, cubeEBO_;
    unsigned int planeVAO_, planeVBO_, planeEBO_;
    Shader *shader_;

    FPSCamera fpsCamera_;
    InputHandler *fpsInputHandler_;
    ArcballCamera arcballCamera_;
    InputHandler *arcballInputHandler_;

    CameraType activeCameraType_;
    InputHandler *activeInputHandler_;

    // Bullet physics
    btCollisionConfiguration* b3CollisionConfiguration_;
    btCollisionDispatcher* b3Dispatcher_;
    btBroadphaseInterface* b3Broadphase_;
    btConstraintSolver* b3Solver_;
    btDynamicsWorld* b3World_;

    // Cube physics object
    btDefaultMotionState* b3CubeMotionState_;
    btDefaultMotionState* b3PlaneMotionState_;
};

#endif // APP_HPP
