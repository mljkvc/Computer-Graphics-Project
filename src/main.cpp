#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    bool blicaj = false;
    bool migavacL = false;
    bool migavacD = false;
    bool move = false;
    bool skySwitch = false;
    bool hdrSwitch = false;

    glm::vec3 putPosition = glm::vec3(-80.0f, 0.0f, 0.0f);
    float putScale = 1.0f;

    glm::vec3 nisanPosition1 = glm::vec3(11.0f, 1.57f, -1.1f);
    float nisanScale1 = 0.7f;
    glm::vec3 nisanPosition2 = glm::vec3(0.0f, 1.65f, 0.7f);
    float nisanScale2 = 1.5f;
    glm::vec3 nisanPosition3 = glm::vec3(-11.0f, 1.57f, -1.65f);
    float nisanScale3 = 0.7f;
    glm::vec3 nisanPosition4 = glm::vec3(-25.0f, 1.65f, 0.72f);
    float nisanScale4 = 1.5f;

    glm::vec3 drvoPosition = glm::vec3(-80.0f, 0.4f, 5.0f);
    float drvoScale = 0.4f;

    glm::vec3 pwrlPosition = glm::vec3(-64.8f, 0.8f, -5.0f);
    float pwrlScale = 0.6f;

    glm::vec3 lampPosition = glm::vec3(-90.0f, 0.8f, 3.0f);
    float lampScale = 0.1f;

    glm::vec3 travaPosition = glm::vec3(-100.0f, 0.35f, 10.0f);
    glm::vec3 trava2Position = glm::vec3(-100.0f, 0.35f, -10.5f);
    float travaScale = 1.0f;

    glm::vec3 zgradePosition = glm::vec3(-140.0f, 17.0f, 63.0f);
    float zgradeScale = 2.0f;

    glm::vec3 planinaPosition = glm::vec3(80.0f, 0.0f, 0.0f);
    float planinaScale = 30.0f;

    glm::vec3 terrainPosition = glm::vec3(0.0f, 4.05f, 0.0f);
    glm::vec3 terrain1Position = glm::vec3(-214.0f, 4.05f, 0.0f);
    float terrainScale = 30.0f;

    SpotLight spotLight;
    SpotLight spotLight1;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

unsigned int colorBuffers[2];
unsigned int rboDepth;
unsigned int pingpongColorbuffers[2];

float speed = 7.0f; // brzina puta
float speedZgrada = 4.5f; // brzina zgrada


int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //msaa
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rollers", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    //blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader hdrShader("resources/shaders/hdr.vs", "resources/shaders/hdr.fs");
    Shader bloomShader("resources/shaders/bloom.vs", "resources/shaders/bloom.fs");
    Shader blurShader("resources/shaders/blur.vs", "resources/shaders/blur.fs");

    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    //HDR
    float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };


    // load models
    // -----------
    stbi_set_flip_vertically_on_load(false);

    Model put("resources/objects/okoloputnici/road/road.obj");
    put.SetShaderTextureNamePrefix("material.");

    Model auto1("resources/objects/Auti/roze_nissan/sx180.obj");
    auto1.SetShaderTextureNamePrefix("material.");
    Model auto2("resources/objects/Auti/beli_nissan_s15/s15.obj");
    auto2.SetShaderTextureNamePrefix("material.");
    Model auto3("resources/objects/Auti/porsche_911_rwb/porche.obj");
    auto3.SetShaderTextureNamePrefix("material.");
    Model auto4("resources/objects/Auti/nissan_240sx_crveni/240sx.obj");
    auto4.SetShaderTextureNamePrefix("material.");

    Model drva("resources/objects/Priroda/custom drva/drvece.obj");
    drva.SetShaderTextureNamePrefix("material.");

    Model powerline("resources/objects/okoloputnici/powerline/scene.gltf");
    powerline.SetShaderTextureNamePrefix("material.");

    Model lamp("resources/objects/okoloputnici/street_lamp/scene.gltf");
    lamp.SetShaderTextureNamePrefix("material.");

    Model trava("resources/objects/Priroda/trava/textures/trava.obj");
    trava.SetShaderTextureNamePrefix("material.");

    Model zgrada("resources/objects/okoloputnici/zgrade/scene.gltf");
    zgrada.SetShaderTextureNamePrefix("material.");

    Model planina("resources/objects/Priroda/planine/scene.gltf");
    planina.SetShaderTextureNamePrefix("material.");

    Model terrain("resources/objects/Priroda/brda/scene.gltf");
    terrain.SetShaderTextureNamePrefix("material.");


    //===============


    //directional light
    DirLight dirLight;
    dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3(0.5f);
    dirLight.diffuse = glm::vec3(0.4f);
    dirLight.specular = glm::vec3(0.5f);

    // Spotlight za farova auta
    SpotLight& spotLight = programState->spotLight;
    spotLight.ambient = glm::vec3(1.0, 1.0, 1.0);
    spotLight.diffuse = glm::vec3(0.3, 0.3, 0.9);
    spotLight.specular = glm::vec3(1.0, 1.0, 1.0);
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
    spotLight.quadratic = 0.032f;
    spotLight.cutOff = glm::cos(glm::radians(5.0f));
    spotLight.outerCutOff = glm::cos(glm::radians(10.0f));

    SpotLight& spotLight1 = programState->spotLight1;
    spotLight1.ambient = glm::vec3(1.0, 1.0, 1.0);
    spotLight1.diffuse = glm::vec3(0.3, 0.3, 0.9);
    spotLight1.specular = glm::vec3(1.0, 1.0, 1.0);
    spotLight1.constant = 1.0f;
    spotLight1.linear = 0.09f;
    spotLight1.quadratic = 0.032f;
    spotLight1.cutOff = glm::cos(glm::radians(5.0f));
    spotLight1.outerCutOff = glm::cos(glm::radians(10.0f));


    //fog
    float fogDensity = 5.0f;
    float fogStart = 30.0f;
    float fogEnd = 100.0f;
    glm::vec3 fogColor = glm::vec3(0.7f, 0.7f, 0.7f);


    //hdr-------------
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    unsigned int pingpongFBO[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }


    // setup plane VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    //-----------------------


    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // shader configuration
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    bool prekidac = false;
    bool prekidac2 = true;
    bool prekidac3 = true;
    bool prekidac4 = false;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        //donja granica za kameru
        if (programState->camera.Position.y < 1.5f)
            programState->camera.Position.y = 1.5f;

        // skybox textures
        stbi_set_flip_vertically_on_load(false);
        vector<std::string> faces;
        if(programState->skySwitch) {
            faces = {
                    FileSystem::getPath("resources/textures/skybox1/right.png"),
                    FileSystem::getPath("resources/textures/skybox1/left.png"),
                    FileSystem::getPath("resources/textures/skybox1/top.png"),
                    FileSystem::getPath("resources/textures/skybox1/bottom.png"),
                    FileSystem::getPath("resources/textures/skybox1/front.png"),
                    FileSystem::getPath("resources/textures/skybox1/back.png")
            };
        }
        else {
            faces = {
                    FileSystem::getPath("resources/textures/skybox2/right.png"),
                    FileSystem::getPath("resources/textures/skybox2/left.png"),
                    FileSystem::getPath("resources/textures/skybox2/top.png"),
                    FileSystem::getPath("resources/textures/skybox2/bottom.png"),
                    FileSystem::getPath("resources/textures/skybox2/front.png"),
                    FileSystem::getPath("resources/textures/skybox2/back.png")
            };
        }
        unsigned int cubemapTexture = loadCubemap(faces);

        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        //fog
        ourShader.setFloat("fogDensity", fogDensity);
        ourShader.setFloat("fogStart", fogStart);
        ourShader.setFloat("fogEnd", fogEnd);
        ourShader.setVec3("fogColor", fogColor);
        ourShader.setFloat("transparent", 1.0f);


        //dir light
        ourShader.setVec3("dirLight.direction", dirLight.direction);
        ourShader.setVec3("dirLight.ambient", dirLight.ambient);
        ourShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        ourShader.setVec3("dirLight.specular", dirLight.specular);

        //funkcionalnost dugih svetala
        if(programState->blicaj){
            spotLight.ambient = glm::vec3(5.0, 5.0, 5.0);
            spotLight1.ambient = glm::vec3(5.0, 5.0, 5.0);
        }
        else{
            spotLight.ambient = glm::vec3(1.0, 1.0, 1.0);
            spotLight1.ambient = glm::vec3(1.0, 1.0, 1.0);
        }

        //funkcionalnost migavaca
        if(programState->migavacL){
            if(int(currentFrame) % 2) {
                spotLight.diffuse = glm::vec3(15.0, 10.0, 0.0);
                spotLight.specular = glm::vec3(15.0, 10.0, 0.0);
            }
            else{
                spotLight.diffuse = glm::vec3(0.3, 0.3, 0.9);
                spotLight.specular = glm::vec3(1.0, 1.0, 1.0);
            }
        }
        else{
            spotLight.diffuse = glm::vec3(0.3, 0.3, 0.9);
            spotLight.specular = glm::vec3(1.0, 1.0, 1.0);
        }

        if(programState->migavacD){
            if(int(currentFrame) % 2) {
                spotLight1.diffuse = glm::vec3(15.0, 10.0, 0.0);
                spotLight1.specular = glm::vec3(15.0, 10.0, 0.0);
            }
            else{
                spotLight1.diffuse = glm::vec3(0.3, 0.3, 0.9);
                spotLight1.specular = glm::vec3(1.0, 1.0, 1.0);
            }
        }
        else{
            spotLight1.diffuse = glm::vec3(0.3, 0.3, 0.9);
            spotLight1.specular = glm::vec3(1.0, 1.0, 1.0);
        }


        // Spotlight
        ourShader.setVec3("spotLight.direction", -1.0f, -0.01f, 0.0f);
        ourShader.setVec3("spotLight.ambient", spotLight.ambient);
        ourShader.setVec3("spotLight.diffuse", spotLight.diffuse);
        ourShader.setVec3("spotLight.specular", spotLight.specular);
        ourShader.setFloat("spotLight.constant", spotLight.constant);
        ourShader.setFloat("spotLight.linear", spotLight.linear);
        ourShader.setFloat("spotLight.quadratic", spotLight.quadratic);
        ourShader.setFloat("spotLight.cutOff", spotLight.cutOff);
        ourShader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);

        ourShader.setVec3("spotLight1.direction", -1.0f, -0.01f, 0.0f);
        ourShader.setVec3("spotLight1.ambient", spotLight1.ambient);
        ourShader.setVec3("spotLight1.diffuse", spotLight1.diffuse);
        ourShader.setVec3("spotLight1.specular", spotLight1.specular);
        ourShader.setFloat("spotLight1.constant", spotLight1.constant);
        ourShader.setFloat("spotLight1.linear", spotLight1.linear);
        ourShader.setFloat("spotLight1.quadratic", spotLight1.quadratic);
        ourShader.setFloat("spotLight1.cutOff", spotLight1.cutOff);
        ourShader.setFloat("spotLight1.outerCutOff", spotLight1.outerCutOff);


        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 10.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f + 69.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        //pomeranje auta
        if(programState->move) {

            float napred = 5.0f;
            float nazad = 3.0f;


            //1
            if (prekidac)
                programState->nisanPosition1.x -= napred * deltaTime;
            if(!prekidac)
                programState->nisanPosition1.x += nazad * deltaTime;
            if (programState->nisanPosition1.x <= 6.0f)
                prekidac = false;
            if (programState->nisanPosition1.x >= 17.0f)
                prekidac = true;


            //2
            if (prekidac2)
                programState->nisanPosition2.x -= napred * deltaTime;
            if(!prekidac2)
                programState->nisanPosition2.x += nazad * deltaTime;
            if (programState->nisanPosition2.x <= -5.0f) {
                prekidac2 = false;
            }
            if (programState->nisanPosition2.x >= 5.0f)
                prekidac2 = true;


            programState->nisanPosition4.z = sin(currentFrame * 1.2f) / 2;

            //3
            if (prekidac3)
                programState->nisanPosition3.x -= napred * deltaTime;
            if(!prekidac3)
                programState->nisanPosition3.x += nazad * deltaTime;
            if (programState->nisanPosition3.x <= -17.0f) {
                prekidac3 = false;
            }
            if (programState->nisanPosition3.x >= -6.0f)
                prekidac3 = true;


            //4
            if (prekidac4)
                programState->nisanPosition4 .x -= napred * deltaTime;
            if(!prekidac4)
                programState->nisanPosition4 .x += nazad * deltaTime;
            if (programState->nisanPosition4.x <= -33.0f) {
                prekidac4 = false;
            }
            if (programState->nisanPosition4.x >= -18.0f)
                prekidac4 = true;



        }
        ourShader.setVec3("spotLight.position", programState->nisanPosition3 + glm::vec3(-0.9,0.06,0.48));
        ourShader.setVec3("spotLight1.position", programState->nisanPosition3 + glm::vec3(-0.9,0.06,-0.48));



        // renderovanje puta
        if(programState->move)
            programState->putPosition.x += speed * deltaTime;

        //funkcionalnost puta
        if (programState->putPosition.x >= -49.0f)
            programState->putPosition.x = -80.0f;

        model = glm::mat4(1.0f);
        for (int i = 0; i < 6; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, programState->putPosition + glm::vec3(31.0f * float(i), 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(programState->putScale));
            ourShader.setMat4("model", model);
            put.Draw(ourShader);
        }


        //1. auto
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->nisanPosition1);
        model = glm::scale(model, glm::vec3(programState->nisanScale1));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        auto1.Draw(ourShader);

        //2. auto
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->nisanPosition2);
        model = glm::scale(model, glm::vec3(programState->nisanScale2));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        auto2.Draw(ourShader);

        //3. auto
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->nisanPosition3);
        model = glm::scale(model, glm::vec3(programState->nisanScale3));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        auto3.Draw(ourShader);

        //4. auto
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->nisanPosition4);
        model = glm::scale(model, glm::vec3(programState->nisanScale4));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        auto4.Draw(ourShader);


        //renderovanje drveca
        if(programState->move)
            programState->drvoPosition.x += speed * deltaTime;
        if (programState->drvoPosition.x >= -40.0f)
            programState->drvoPosition.x = -80.0f;

        model = glm::mat4(1.0f);
        for (int i = 0; i < 5; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model,programState->drvoPosition + glm::vec3(40.0f * float(i), 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(programState->drvoScale));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            ourShader.setMat4("model", model);
            drva.Draw(ourShader);
        }



        //render zgrada
        if(programState->move)
            programState->zgradePosition.x += speedZgrada * deltaTime;
        if (programState->zgradePosition.x >= -70.0f)
            programState->zgradePosition.x = -140.0f;

        model = glm::mat4(1.0f);
        for (int i = 0; i < 4; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model,programState->zgradePosition + glm::vec3(70.0f * float(i), 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(programState->zgradeScale));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            ourShader.setMat4("model", model);
            zgrada.Draw(ourShader);
        }

        //render stubova
        if(programState->move)
            programState->pwrlPosition.x += speed * deltaTime;
        if (programState->pwrlPosition.x >= -48.6f)
            programState->pwrlPosition.x = -64.8f;

        model = glm::mat4(1.0f);
        for (int i = 0; i < 10; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model,programState->pwrlPosition + glm::vec3(16.2f * float(i), 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(programState->pwrlScale));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            ourShader.setMat4("model", model);
            powerline.Draw(ourShader);
        }


        //render lampi
        if(programState->move)
            programState->lampPosition.x += speed * deltaTime;
        if (programState->lampPosition.x >= -60.0f)
            programState->lampPosition.x = -90.0f;

        model = glm::mat4(1.0f);
        for (int i = 0; i < 7; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model,programState->lampPosition + glm::vec3(30.0f * float(i), 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(programState->lampScale));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            ourShader.setMat4("model", model);
            lamp.Draw(ourShader);
        }


        //spotlight za lampu
        std::vector<SpotLight> spotLights;
        for (int i = 0; i < 7; ++i) {
            SpotLight LampLight;
            LampLight.position = programState->lampPosition + glm::vec3(30.0f * float(i), 6.8f, -3.4f);
            LampLight.ambient = glm::vec3(1.0, 1.0, 1.0);
            LampLight.diffuse = glm::vec3(1.0, 0.7, 0.0);
            LampLight.specular = glm::vec3(1.0, 0.7, 0.0);
            LampLight.constant = 1.0f;
            LampLight.linear = 0.09f;
            LampLight.quadratic = 0.032f;
            LampLight.cutOff = glm::cos(glm::radians(10.0f));
            LampLight.outerCutOff = glm::cos(glm::radians(25.0f));
            spotLights.push_back(LampLight);
        }

        for (size_t i = 0; i < spotLights.size(); ++i) {
            ourShader.setVec3("spotLights[" + std::to_string(i) + "].direction", 0.0f, -1.0f, 0.0f);
            ourShader.setVec3("spotLights[" + std::to_string(i) + "].ambient", spotLights[i].ambient);
            ourShader.setVec3("spotLights[" + std::to_string(i) + "].diffuse", spotLights[i].diffuse);
            ourShader.setVec3("spotLights[" + std::to_string(i) + "].specular", spotLights[i].specular);
            ourShader.setFloat("spotLights[" + std::to_string(i) + "].constant", spotLights[i].constant);
            ourShader.setFloat("spotLights[" + std::to_string(i) + "].linear", spotLights[i].linear);
            ourShader.setFloat("spotLights[" + std::to_string(i) + "].quadratic", spotLights[i].quadratic);
            ourShader.setFloat("spotLights[" + std::to_string(i) + "].cutOff", spotLights[i].cutOff);
            ourShader.setFloat("spotLights[" + std::to_string(i) + "].outerCutOff", spotLights[i].outerCutOff);
            ourShader.setVec3("spotLights[" + std::to_string(i) + "].position", spotLights[i].position);
        }

        //render trave------------------------------------------

        //pomeranje trave
        if(programState->move) {
            programState->travaPosition.x += speed * deltaTime;
            programState->trava2Position.x += speed * deltaTime;
        }
        if (programState->travaPosition.x >= -40.0f)
            programState->travaPosition.x = -100.0f;
        if (programState->trava2Position.x >= -40.0f)
            programState->trava2Position.x = -100.0f;

        glDisable(GL_CULL_FACE);
        model = glm::mat4(1.0f);
        //desno
        for (int i = 0; i < 3; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model,programState->travaPosition + glm::vec3(60.0f * float(i), 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(programState->travaScale));
            ourShader.setMat4("model", model);
            trava.Draw(ourShader);
        }
        //levo
        model = glm::mat4(1.0f);
        for (int i = 0; i < 3; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model,programState->trava2Position + glm::vec3(60.0f * float(i), 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(programState->travaScale));
            ourShader.setMat4("model", model);
            trava.Draw(ourShader);
        }

        //render planine-----------------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->planinaPosition);
        model = glm::scale(model, glm::vec3(programState->planinaScale));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        planina.Draw(ourShader);

        //render brda
        if(programState->move) {
            programState->terrainPosition.x += speed * deltaTime;
            programState->terrain1Position.x += speed * deltaTime;
        }
        if (programState->terrainPosition.x >= 214.0f)
            programState->terrainPosition.x = -214.0f;
        if (programState->terrain1Position.x >= 214.0f)
            programState->terrain1Position.x = -214.0f;

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->terrainPosition);
        model = glm::scale(model, glm::vec3(programState->terrainScale));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        terrain.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->terrain1Position);
        model = glm::scale(model, glm::vec3(programState->terrainScale));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        terrain.Draw(ourShader);


        glEnable(GL_CULL_FACE);


        // draw skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(programState->camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        //HDR
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 5;
        blurShader.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bloomShader.use();
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        bloomShader.setInt("bloom", programState->hdrSwitch);
        bloomShader.setFloat("exposure", 0.5f);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);




        DrawImGui(programState);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // deallocate
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &quadVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(DOWN, deltaTime);

    //brzina trake
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        speed += 0.5;
        speedZgrada += 0.5;
        if(speed >= 197.0f)
            speed = 197.0f;
        if(speedZgrada >= 194.5f)
            speedZgrada = 194.5f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        speed -= 0.5;
        speedZgrada -= 0.5;
        if(speed <= 2.0f)
            speed = 2.0f;
        if(speedZgrada <= 2.5f)
            speedZgrada = 2.5f;

    }
}


void hdrResize(int width, int height) {
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void bloomResize(int width, int height) {
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    hdrResize(width, height);
    bloomResize(width, height);
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //fps info
    ImVec4 textColor;
    float fps = ImGui::GetIO().Framerate;
    if (fps > 55.0f)
        textColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    else if (fps >= 40.0f && fps <= 55.0f)
        textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
    else
        textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 window_pos = ImVec2(io.DisplaySize.x - 85, 5);
    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
    ImGui::TextColored(textColor, "FPS: %.1f", fps);
    ImGui::End();

    //camera info
//    if (programState->ImGuiEnabled) {
//        ImGui::Begin("Camera info");
//        const Camera &c = programState->camera;
//        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
//        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
//        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
//        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
//        ImGui::End();
//    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_B && action == GLFW_PRESS) {
        programState->blicaj = !programState->blicaj;
    }

    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        programState->migavacL = !programState->migavacL;
    }

    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        programState->migavacD = !programState->migavacD;
    }

    if(key == GLFW_KEY_M && action == GLFW_PRESS) {
        programState->move = !programState->move;
    }

    if(key == GLFW_KEY_Q && action == GLFW_PRESS) {
        programState->skySwitch = !programState->skySwitch;
    }
    if(key == GLFW_KEY_H && action == GLFW_PRESS) {
        programState->hdrSwitch = !programState->hdrSwitch;
    }
    if(key == GLFW_KEY_H && action == GLFW_PRESS) {
        programState->hdrSwitch = !programState->hdrSwitch;
    }
    if(key == GLFW_KEY_H && action == GLFW_PRESS) {
        programState->hdrSwitch = !programState->hdrSwitch;
    }


    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
//            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

//skybox
unsigned int loadCubemap(vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++){
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else{
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
