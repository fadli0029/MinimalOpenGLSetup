#ifndef APP_HPP
#define APP_HPP

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <random>
#include "shader.hpp"
#include "cameras/fps_camera.hpp"
#include "input_handlers/fps_input_handler.hpp"

class App {
public:
    App(int width, int height, const std::string& title, const char* vertexShader, const char* fragmentShader) {
        screenWidth = width;
        screenHeight = height;
        windowTitle = title;
        window = nullptr;
        context = nullptr;
        quit = false;
        fpsCamera = FPSCamera(glm::vec3(0.0f, 0.0f, 3.0f));
        inputHandler = new FPSInputHandler(fpsCamera);

        deltaTime = 0.0f;
        lastFrame = 0.0f;

        vertexShaderPath = vertexShader;
        fragmentShaderPath = fragmentShader;
    }

    ~App() {
        delete inputHandler;
    }

    bool Initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL2 could not initialize video subsystem." << std::endl;
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        window = SDL_CreateWindow(
            windowTitle.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            screenWidth, screenHeight,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );
        if (!window) {
            std::cerr << "SDL_Window could not be created." << std::endl;
            return false;
        }

        context = SDL_GL_CreateContext(window);
        if (!context) {
            std::cerr << "OpenGL context could not be created." << std::endl;
            return false;
        }

        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            std::cerr << "GLAD could not initialize OpenGL context." << std::endl;
            return false;
        }

        SDL_SetRelativeMouseMode(SDL_TRUE);

        GetOpenGLVersionInfo();
        InitOpenGL();
        return true;
    }

    void Run() {
        while (!quit) {
            float currentFrame = SDL_GetTicks() / 1000.0f;
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            ProcessInput();
            Update();
            Render();
            SDL_GL_SwapWindow(window);
        }
        CleanUp();
    }

private:
    void GetOpenGLVersionInfo() {
        std::cout << "=============================================================" << std::endl;
        std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "Shader Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        std::cout << "=============================================================" << std::endl;
        std::cout << std::endl;
    }

    void InitOpenGL() {
        shader = new Shader(vertexShaderPath, fragmentShaderPath);

        // Generate random colors for the vertices every build and run
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        float vertices[] = {
            // positions           // colors
            -0.5f, -0.5f, -0.5f,   float(dis(gen)), float(dis(gen)), float(dis(gen)),
             0.5f, -0.5f, -0.5f,   float(dis(gen)), float(dis(gen)), float(dis(gen)),
             0.5f,  0.5f, -0.5f,   float(dis(gen)), float(dis(gen)), float(dis(gen)),
            -0.5f,  0.5f, -0.5f,   float(dis(gen)), float(dis(gen)), float(dis(gen)),
            -0.5f, -0.5f,  0.5f,   float(dis(gen)), float(dis(gen)), float(dis(gen)),
             0.5f, -0.5f,  0.5f,   float(dis(gen)), float(dis(gen)), float(dis(gen)),
             0.5f,  0.5f,  0.5f,   float(dis(gen)), float(dis(gen)), float(dis(gen)),
            -0.5f,  0.5f,  0.5f,   float(dis(gen)), float(dis(gen)), float(dis(gen))
        };

        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,    // front face
            4, 5, 6, 6, 7, 4,    // back face
            0, 1, 5, 5, 4, 0,    // bottom face
            2, 3, 7, 7, 6, 2,    // top face
            0, 3, 7, 7, 4, 0,    // left face
            1, 2, 6, 6, 5, 1     // right face
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glEnable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }

    void ProcessInput() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                std::cout << "SDL_QUIT event triggered." << std::endl;
                quit = true;
            } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                inputHandler->HandleKeyboardInput(e, deltaTime);
            } else if (e.type == SDL_MOUSEMOTION) {
                inputHandler->HandleMouseMotion(e);
            } else if (e.type == SDL_MOUSEWHEEL) {
                inputHandler->HandleMouseWheel(e);
            }
        }
    }

    void Update() {
        // Update logic can be implemented here
    }

    void Render() {
        // 1. Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. Use the shader program
        shader->use();

        // 3. Set variables (uniforms) in the shaders (ex: the model, view and projection matrices)
        // (a) Set the camera view and projection matrices
        glm::mat4 view = fpsCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(fpsCamera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setMat4("model", model);

        // (b) Other stuff to do to the shaders...
        // ...

        // 4. Render the cube (or whatever object you have)
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void CleanUp() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete shader;
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    int screenWidth;
    int screenHeight;

    float deltaTime;
    float lastFrame;

    const char* vertexShaderPath;
    const char* fragmentShaderPath;

    bool quit;
    std::string windowTitle;
    SDL_Window* window;
    SDL_GLContext context;

    unsigned int VAO, VBO, EBO;
    Shader* shader;

    FPSCamera fpsCamera;
    InputHandler* inputHandler;
};

#endif // APP_HPP
