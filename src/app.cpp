#include "app/app.hpp"

App::App(int width, int height, const std::string& title, const char* vertexShader, const char* fragmentShader) {
    screenWidth_ = width;
    screenHeight_ = height;
    windowTitle_ = title;
    vertexShaderPath_ = vertexShader;
    fragmentShaderPath_ = fragmentShader;

    window_ = nullptr;
    context_ = nullptr;
    quit_ = false;

    fpsCamera_ = FPSCamera(glm::vec3(0.0f, 0.0f, 3.0f));
    fpsInputHandler_ = new FPSInputHandler(fpsCamera_);

    arcballCamera_ = ArcballCamera(glm::vec3(0.0f, 0.0f, 5.0f));
    arcballInputHandler_ = new ArcballInputHandler(arcballCamera_);

    deltaTime_ = 0.0f;
    lastFrame_ = 0.0f;

    // Default to FPS camera
    activeCameraType_ = CameraType::FPS;
    activeInputHandler_ = fpsInputHandler_;
}

App::~App() {
    delete fpsInputHandler_;
    delete arcballInputHandler_;
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

    window_ = SDL_CreateWindow(
        windowTitle_.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth_, screenHeight_,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!window_) {
        std::cerr << "SDL_Window could not be created." << std::endl;
        return false;
    }

    context_ = SDL_GL_CreateContext(window_);
    if (!context_) {
        std::cerr << "OpenGL context could not be created." << std::endl;
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "GLAD could not initialize OpenGL context." << std::endl;
        return false;
    }

    InitPhysics();
    GetOpenGLVersionInfo();
    InitOpenGL();
    return true;
}

void App::Run() {
    while (!quit_) {
        float currentFrame = SDL_GetTicks() / 1000.0f; // in seconds
        deltaTime_ = currentFrame - lastFrame_;
        lastFrame_ = currentFrame;

        ProcessInput();
        Update();
        Render();
        SDL_GL_SwapWindow(window_);
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
    shader_ = new Shader(vertexShaderPath_, fragmentShaderPath_);

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
    glGenVertexArrays(1, &cubeVAO_);
    glGenBuffers(1, &cubeVBO_);
    glGenBuffers(1, &cubeEBO_);

    glBindVertexArray(cubeVAO_);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Plane VAO
    glGenVertexArrays(1, &planeVAO_);
    glGenBuffers(1, &planeVBO_);
    glGenBuffers(1, &planeEBO_);

    glBindVertexArray(planeVAO_);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO_);
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
            quit_ = true;
        }
        else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_c) {
                SwitchCamera();
            }
        }
        else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
            UpdateViewport(e.window.data1, e.window.data2);
        }
        activeInputHandler_->HandleInput(e, deltaTime_);
    }
}

void App::Update() {
    if (activeCameraType_ == CameraType::FPS) {
        dynamic_cast<FPSInputHandler*>(activeInputHandler_)->Update(deltaTime_);
    }
    b3World_->stepSimulation(deltaTime_, 10);
}

void App::Render() {
    // 1. Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Use the shader program
    shader_->use();

    // 3. Set variables (uniforms) in the shaders (ex: the model, view and projection matrices)
    glm::mat4 view = (activeCameraType_ == CameraType::FPS)
        ? fpsCamera_.GetViewMatrix()
        : arcballCamera_.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians((activeCameraType_ == CameraType::FPS)
        ? fpsCamera_.Zoom
        : arcballCamera_.Zoom), (float)screenWidth_ / (float)screenHeight_, 0.1f, 100.0f);
    shader_->setMat4("view", view);
    shader_->setMat4("projection", projection);

    // Render the cube with the Bullet Physics transformation
    float matrix[16];
    if (b3CubeMotionState_) {
        btTransform trans;
        b3CubeMotionState_->getWorldTransform(trans);
        trans.getOpenGLMatrix(matrix);
    }
    glm::mat4 model = glm::make_mat4(matrix);
    shader_->setMat4("model", model);
    // glm::mat4 model = glm::mat4(1.0f);
    // shader_->setMat4("model", model);

    glBindVertexArray(cubeVAO_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Render the plane
    model = glm::mat4(1.0f);
    shader_->setMat4("model", model);

    glBindVertexArray(planeVAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void App::SwitchCamera() {
    if (activeCameraType_ == CameraType::FPS) {
        // Sync Arcball camera with the current state of the FPS camera
        arcballCamera_.Position = fpsCamera_.Position;
        arcballCamera_.Target = fpsCamera_.Position + fpsCamera_.Front;
        arcballCamera_.Distance = glm::distance(fpsCamera_.Position, arcballCamera_.Target);

        activeCameraType_ = CameraType::Arcball;
        activeInputHandler_ = arcballInputHandler_;

        SDL_SetRelativeMouseMode(SDL_FALSE);
    } else {
        // Sync FPS camera with the current state of the Arcball camera
        fpsCamera_.Position = arcballCamera_.Position;
        fpsCamera_.Front = glm::normalize(arcballCamera_.Target - arcballCamera_.Position);
        fpsCamera_.Yaw = glm::degrees(atan2(fpsCamera_.Front.z, fpsCamera_.Front.x));
        fpsCamera_.Pitch = glm::degrees(asin(fpsCamera_.Front.y));

        activeCameraType_ = CameraType::FPS;
        activeInputHandler_ = fpsInputHandler_;

        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
}

void App::UpdateViewport(const int &width, const int &height) {
    screenWidth_ = width;
    screenHeight_ = height;
    glViewport(0, 0, screenWidth_, screenHeight_);
}

void App::CleanUp() {
    glDeleteVertexArrays(1, &cubeVAO_);
    glDeleteBuffers(1, &cubeVBO_);
    glDeleteBuffers(1, &cubeEBO_);

    glDeleteVertexArrays(1, &planeVAO_);
    glDeleteBuffers(1, &planeVBO_);
    glDeleteBuffers(1, &planeEBO_);

    delete shader_;
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void App::InitPhysics() {
    b3CollisionConfiguration_ = new btDefaultCollisionConfiguration();
    b3Dispatcher_ = new btCollisionDispatcher(b3CollisionConfiguration_);
    b3Broadphase_ = new btDbvtBroadphase();
    b3Solver_ = new btSequentialImpulseConstraintSolver();
    b3World_ = new btDiscreteDynamicsWorld(b3Dispatcher_, b3Broadphase_, b3Solver_, b3CollisionConfiguration_);

    CreatePhysicsObjects();
}

void App::ShutdownPhysics() {
    delete b3World_;
    delete b3Solver_;
    delete b3Broadphase_;
    delete b3Dispatcher_;
    delete b3CollisionConfiguration_;
}

void App::CreatePhysicsObjects() {
    // Create a box shape of size (1,1,1)
    btBoxShape* boxShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

    // Set box initial pose
    btTransform transform;
    transform.setIdentity(); // init. orientation
    transform.setOrigin(btVector3(0.0f, 30.0f, 0.0f)); // init. position

    // Create a motion state
    b3CubeMotionState_ = new btDefaultMotionState(transform);

    // Create the rigid body construction info object, giving it a
    // mass of 1, the motion state, and the shape
    btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, b3CubeMotionState_, boxShape);
    btRigidBody* b3RigidBody = new btRigidBody(rbInfo);

    // Inform our world that we just created a new rigid body for it to manage
    b3World_->addRigidBody(b3RigidBody);
}
