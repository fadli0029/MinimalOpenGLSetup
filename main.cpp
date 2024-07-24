#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include "shader.hpp"

// Shaders source code
const char* vertexShaderPath = "shaders/vert.vs";
const char* fragmentShaderPath = "shaders/frag.fs";

class App {
  public:
      App(int width, int height, const std::string& title)
          : screenWidth(width), screenHeight(height), windowTitle(title), window(nullptr), context(nullptr), quit(false) {}

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
              SDL_WINDOW_OPENGL
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

          GetOpenGLVersionInfo();
          InitOpenGL();
          return true;
      }

      void Run() {
          while (!quit) {
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

          float vertices[] = {
              0.0f,  0.5f, 0.0f,
             -0.5f, -0.5f, 0.0f,
              0.5f, -0.5f, 0.0f
          };

          glGenVertexArrays(1, &VAO);
          glGenBuffers(1, &VBO);

          glBindVertexArray(VAO);

          glBindBuffer(GL_ARRAY_BUFFER, VBO);
          glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
          glEnableVertexAttribArray(0);

          glBindBuffer(GL_ARRAY_BUFFER, 0);
          glBindVertexArray(0);
      }

      void ProcessInput() {
          SDL_Event e;
          while (SDL_PollEvent(&e)) {
              if (e.type == SDL_QUIT) {
                  std::cout << "SDL_QUIT event triggered." << std::endl;
                  quit = true;
              }
          }
      }

      void Update() {
          // Update logic can be implemented here
          float timeValue = SDL_GetTicks() / 1000.0f;
          float greenValue = sin(timeValue) / 2.0f + 0.5f;
          int vertexColorLocation = glGetUniformLocation(shader->ID, "dataColor");
          glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
      }

      void Render() {
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          shader->use();
          glBindVertexArray(VAO);
          glDrawArrays(GL_TRIANGLES, 0, 3);
      }

      void CleanUp() {
          glDeleteVertexArrays(1, &VAO);
          glDeleteBuffers(1, &VBO);
          delete shader;
          SDL_GL_DeleteContext(context);
          SDL_DestroyWindow(window);
          SDL_Quit();
      }

      int screenWidth;
      int screenHeight;

      std::string windowTitle;

      SDL_Window* window;
      SDL_GLContext context;

      bool quit;
      unsigned int VAO, VBO;
      Shader* shader;
};

int main() {
    App app(800, 600, "OpenGL Window");
    if (app.Initialize()) {
        app.Run();
    } else {
        std::cerr << "Failed to initialize the application." << std::endl;
    }
    return 0;
}
