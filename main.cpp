#include "app.hpp"

int main() {
    const char* vertexShader = "shaders/vert.vs";
    const char* fragmentShader = "shaders/frag.fs";

    App app(800, 600, "OpenGL Window", vertexShader, fragmentShader);
    if (app.Initialize()) {
        app.Run();
    } else {
        std::cerr << "Failed to initialize the application." << std::endl;
    }
    return 0;
}
