#include "app/app.hpp"

App::App(int width, int height, const std::string& title, const char* vertexShader, const char* fragmentShader) {
    screenWidth = width;
    screenHeight = height;
    windowTitle = title;
    vertexShaderPath = vertexShader;
    fragmentShaderPath = fragmentShader;

    window = nullptr;
    context = nullptr;
    quit = false;

    fpsCamera = FPSCamera(glm::vec3(0.0f, 0.0f, 3.0f));
    fpsInputHandler = new FPSInputHandler(fpsCamera);

    arcballCamera = ArcballCamera(glm::vec3(0.0f, 0.0f, 5.0f));
    arcballInputHandler = new ArcballInputHandler(arcballCamera);

    deltaTime = 0.0f;
    lastFrame = 0.0f;

    // Default to FPS camera
    activeCameraType = CameraType::FPS;
    activeInputHandler = fpsInputHandler;
}

App::~App() {
    delete fpsInputHandler;
    delete arcballInputHandler;
}

bool App::Initialize() {
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

    GetOpenGLVersionInfo();
    InitOpenGL();

    // TODO: Init physics here?
    return true;
}

void App::Run() {
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

void App::GetOpenGLVersionInfo() {
    std::cout << "=============================================================" << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shader Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << std::endl;
}

void App::InitOpenGL() {
    shader = new Shader(vertexShaderPath, fragmentShaderPath);

    // Generate random colors for the vertices every build and run
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    float cubeVertices[] = {
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

    unsigned int cubeIndices[] = {
        0, 1, 2, 2, 3, 0,    // front face
        4, 5, 6, 6, 7, 4,    // back face
        0, 1, 5, 5, 4, 0,    // bottom face
        2, 3, 7, 7, 6, 2,    // top face
        0, 3, 7, 7, 4, 0,    // left face
        1, 2, 6, 6, 5, 1     // right face
    };

    float planeVertices[] = {
        // positions            // colors
        -50.0f, -0.5f,  50.0f,  0.8f, 0.8f, 0.8f,
         50.0f, -0.5f,  50.0f,  0.8f, 0.8f, 0.8f,
         50.0f, -0.5f, -50.0f,  0.8f, 0.8f, 0.8f,
        -50.0f, -0.5f, -50.0f,  0.8f, 0.8f, 0.8f
    };

    unsigned int planeIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // Cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glEnable(GL_DEPTH_TEST);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void App::ProcessInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            std::cout << "SDL_QUIT event triggered." << std::endl;
            quit = true;
        }
        else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_c) {
                SwitchCamera();
            }
        }
        else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
            UpdateViewport(e.window.data1, e.window.data2);
        }
        activeInputHandler->HandleInput(e, deltaTime);
    }
}

void App::Update() {
    if (activeCameraType == CameraType::FPS) {
        dynamic_cast<FPSInputHandler*>(activeInputHandler)->Update(deltaTime);
    }
}

void App::Render() {
    // 1. Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Use the shader program
    shader->use();

    // 3. Set variables (uniforms) in the shaders (ex: the model, view and projection matrices)
    // (a) Set the camera view and projection matrices
    glm::mat4 view = (activeCameraType == CameraType::FPS)
        ? fpsCamera.GetViewMatrix()
        : arcballCamera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians((activeCameraType == CameraType::FPS)
        ? fpsCamera.Zoom
        : arcballCamera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Render the cube
    glm::mat4 model = glm::mat4(1.0f);
    shader->setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Render the plane
    model = glm::mat4(1.0f);
    shader->setMat4("model", model);

    glBindVertexArray(planeVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void App::SwitchCamera() {
    if (activeCameraType == CameraType::FPS) {
        // Sync Arcball camera with the current state of the FPS camera
        arcballCamera.Position = fpsCamera.Position;
        arcballCamera.Target = fpsCamera.Position + fpsCamera.Front;
        arcballCamera.Distance = glm::distance(fpsCamera.Position, arcballCamera.Target);

        activeCameraType = CameraType::Arcball;
        activeInputHandler = arcballInputHandler;

        SDL_SetRelativeMouseMode(SDL_FALSE);
    } else {
        // Sync FPS camera with the current state of the Arcball camera
        fpsCamera.Position = arcballCamera.Position;
        fpsCamera.Front = glm::normalize(arcballCamera.Target - arcballCamera.Position);
        fpsCamera.Yaw = glm::degrees(atan2(fpsCamera.Front.z, fpsCamera.Front.x));
        fpsCamera.Pitch = glm::degrees(asin(fpsCamera.Front.y));

        activeCameraType = CameraType::FPS;
        activeInputHandler = fpsInputHandler;

        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
}

void App::UpdateViewport(const int &width, const int &height) {
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, screenWidth, screenHeight);
}

void App::CleanUp() {
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &planeEBO);

    delete shader;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
