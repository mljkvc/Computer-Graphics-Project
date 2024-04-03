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

unsigned int loadTexture(char const * path);

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

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
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

    glm::vec3 putPosition = glm::vec3(-80.0f, 0.0f, 0.0f);
    float putScale = 1.0f;

    glm::vec3 nisanPosition = glm::vec3(5.0f, 1.65f, 0.7f);
    float nisanScale = 1.5f;
    glm::vec3 nisanPosition2 = glm::vec3(11.0f, 1.65f, -1.1f);
    float nisanScale2 = 1.5f;
    glm::vec3 nisanPosition3 = glm::vec3(-1.0f, 1.65f, -1.1f);
    float nisanScale3 = 0.7f;

    glm::vec3 drvo1Position = glm::vec3(11.0f, 0.8f, 5.0f);
    float drvo1Scale = 0.01f;
    glm::vec3 drvo2Position = glm::vec3(7.0f, 0.8f, 5.0f);
    float drvo2Scale = 0.01f;
    glm::vec3 drvo3Position = glm::vec3(3.0f, 0.8f, 7.0f);
    float drvo3Scale = 0.3f;

    glm::vec3 pwrlPosition = glm::vec3(-64.8f, 0.8f, -5.0f);
    float pwrlScale = 0.6f;

    glm::vec3 lampPosition = glm::vec3(-90.0f, 0.8f, 3.0f);
    float lampScale = 0.1f;

    glm::vec3 travaPosition = glm::vec3(-60.0f, 0.9f, 4.3f);
    glm::vec3 trava2Position = glm::vec3(-60.0f, 0.9f, -10.5f);
    float travaScale = 1.0f;

    glm::vec3 rocksPosition = glm::vec3(-60.0f, 1.04f, -3.0f);
    float rocksScale = 0.8f;

    PointLight pointLight;
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

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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


    //face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");

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


//    pozicija trave
    float planeVertices[] = {
            // positions          // texture Coords
            25.0f, 1.0f,  3.0f,  2.0f, 0.0f,
            -25.0f, 1.0f,  3.0f,  0.0f, 0.0f,
            -25.0f, 1.0f, -3.0f,  0.0f, 6.0f,

            25.0f, 1.0f,  3.0f,  2.0f, 0.0f,
            -25.0f, 1.0f, -3.0f,  0.0f, 6.0f,
            25.0f, 1.0f, -3.0f,  2.0f, 6.0f
    };


    // load models
    // -----------
    stbi_set_flip_vertically_on_load(false);

    Model put("resources/objects/okoloputnici/road/road.obj");
    put.SetShaderTextureNamePrefix("material.");

    Model nisan("resources/objects/Auti/nissan-240sx/SA5HLA5LO5H1RQJ42KKT685IS.obj");
    nisan.SetShaderTextureNamePrefix("material.");
    Model silvia("resources/objects/Auti/nissan-s15/5B0PLJOVYVIQBVNHBTYILMRIV.obj");
    nisan.SetShaderTextureNamePrefix("material.");
    Model sxNissan("resources/objects/Auti/polovicni auti/nissan_180sx_free/scene.gltf");
    nisan.SetShaderTextureNamePrefix("material.");

    Model drvo1("resources/objects/Priroda/japanese-maple/japaneseMaple.obj");
    drvo1.SetShaderTextureNamePrefix("material.");
    Model drvo2("resources/objects/Priroda/peach-blossom/peachBlossom.obj");
    drvo2.SetShaderTextureNamePrefix("material.");
    Model drvo3("resources/objects/Priroda/drvo/scene.gltf");
    drvo3.SetShaderTextureNamePrefix("material.");

    Model powerline("resources/objects/okoloputnici/powerline/scene.gltf");
    powerline.SetShaderTextureNamePrefix("material.");

    Model lamp("resources/objects/okoloputnici/street_lamp/scene.gltf");
    lamp.SetShaderTextureNamePrefix("material.");

    Model trava("resources/objects/Priroda/green_field/scene.gltf");
    trava.SetShaderTextureNamePrefix("material.");

    Model rocks("resources/objects/Priroda/rocks/kamencici.obj");
    rocks.SetShaderTextureNamePrefix("material.");

    //===============


    //directional light
    DirLight dirLight;
    dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3(0.2f);
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



//ideja za blicanje


    //fog
    float fogDensity = 5.0f;
    float fogStart = 20.0f;
    float fogEnd = 60.0f;
    glm::vec3 fogColor = glm::vec3(0.7f, 0.7f, 0.7f);


    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // load textures
    unsigned int planeTexture = loadTexture(FileSystem::getPath("resources/textures/grass.jpg").c_str());


    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // skybox textures
    stbi_set_flip_vertically_on_load(false);
    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox2/right.png"),
                    FileSystem::getPath("resources/textures/skybox2/left.png"),
                    FileSystem::getPath("resources/textures/skybox2/top.png"),
                    FileSystem::getPath("resources/textures/skybox2/bottom.png"),
                    FileSystem::getPath("resources/textures/skybox2/front.png"),
                    FileSystem::getPath("resources/textures/skybox2/back.png")
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    bool prekidac = false;
    bool prekidac2 = true;
    bool prekidac3 = true;

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


        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        //fog
        ourShader.setFloat("fogDensity", fogDensity);
        ourShader.setFloat("fogStart", fogStart);
        ourShader.setFloat("fogEnd", fogEnd);
        ourShader.setVec3("fogColor", fogColor);


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
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        if(programState->move) {

            float napred = 5.0f;
            float nazad = 3.0f;

            if (prekidac)
                programState->nisanPosition.x -= napred * deltaTime;
            if (!prekidac)
                programState->nisanPosition.x += nazad * deltaTime;

            if (programState->nisanPosition.x <= 0.0f) {
                prekidac = false;
            }
            if (programState->nisanPosition.x >= 9.0f)
                prekidac = true;

            if (prekidac2)
                programState->nisanPosition2.x -= napred * deltaTime;
            if (!prekidac2)
                programState->nisanPosition2.x += nazad * deltaTime;

            if (programState->nisanPosition2.x <= 2.0f) {
                prekidac2 = false;
            }
            if (programState->nisanPosition2.x >= 10.0f)
                prekidac2 = true;

            programState->nisanPosition3.z = sin(currentFrame * 1.2f) / 2;

            if (prekidac3)
                programState->nisanPosition3.x -= napred * deltaTime;
            if (!prekidac3)
                programState->nisanPosition3.x += nazad * deltaTime;

            if (programState->nisanPosition3.x <= -10.0f) {
                prekidac3 = false;
            }
            if (programState->nisanPosition3.x >= -1.0f)
                prekidac3 = true;
        }

        ourShader.setVec3("spotLight.position", programState->nisanPosition3 + glm::vec3(-1.3,-0.03,0.4));
        ourShader.setVec3("spotLight1.position", programState->nisanPosition3 + glm::vec3(-1.3,-0.03,-0.4));



        // renderovanje puta
        float speed = 7.0f; // brzina puta
        if(programState->move)
            programState->putPosition.x += speed * deltaTime;

        //funkcionalnost puta
        if (programState->putPosition.x >= -49.0f)
            programState->putPosition.x = -80.0f;

        glm::mat4 putModel = glm::mat4(1.0f);
        for (int i = 0; i < 6; ++i) {
            putModel = glm::mat4(1.0f);
            putModel = glm::translate(putModel, programState->putPosition + glm::vec3(31.0f * float(i), 0.0f, 0.0f));
            putModel = glm::scale(putModel, glm::vec3(programState->putScale));
            ourShader.setMat4("model", putModel);
            put.Draw(ourShader);
        }


        //renderovanje nisana
        glm::mat4 nisanModel = glm::mat4(1.0f);
        nisanModel = glm::translate(nisanModel,programState->nisanPosition);
        nisanModel = glm::scale(nisanModel, glm::vec3(programState->nisanScale));
        nisanModel = glm::rotate(nisanModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", nisanModel);
        nisan.Draw(ourShader);

        //renderovanje drugog nisana
        glm::mat4 nisanModel2 = glm::mat4(1.0f);
        nisanModel2 = glm::translate(nisanModel2,programState->nisanPosition2);
        nisanModel2 = glm::scale(nisanModel2, glm::vec3(programState->nisanScale2));
        nisanModel2 = glm::rotate(nisanModel2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", nisanModel2);
        silvia.Draw(ourShader);

        //renderovanje treceg nisana
        glm::mat4 nisanModel3 = glm::mat4(1.0f);
        nisanModel3 = glm::translate(nisanModel3,programState->nisanPosition3);
        nisanModel3 = glm::scale(nisanModel3, glm::vec3(programState->nisanScale3));
        nisanModel3 = glm::rotate(nisanModel3, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        nisanModel3 = glm::rotate(nisanModel3, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", nisanModel3);
        sxNissan.Draw(ourShader);


        //renderovanje drveca
        //1
        glm::mat4 drvo1Model = glm::mat4(1.0f);
        drvo1Model = glm::translate(drvo1Model,programState->drvo1Position);
        drvo1Model = glm::scale(drvo1Model, glm::vec3(programState->drvo1Scale));
        drvo1Model = glm::rotate(drvo1Model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", drvo1Model);
        drvo1.Draw(ourShader);
        //2
        glm::mat4 drvo2Model = glm::mat4(1.0f);
        drvo2Model = glm::translate(drvo2Model,programState->drvo2Position);
        drvo2Model = glm::scale(drvo2Model, glm::vec3(programState->drvo2Scale));
        drvo2Model = glm::rotate(drvo2Model, glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", drvo2Model);
        drvo2.Draw(ourShader);
        //3
        glm::mat4 drvo3Model = glm::mat4(1.0f);
        drvo3Model = glm::translate(drvo3Model,programState->drvo3Position);
        drvo3Model = glm::scale(drvo3Model, glm::vec3(programState->drvo3Scale));
        drvo3Model = glm::rotate(drvo3Model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", drvo3Model);
        drvo3.Draw(ourShader);


        //render stubova
        if(programState->move)
            programState->pwrlPosition.x += speed * deltaTime;
        if (programState->pwrlPosition.x >= -48.6f)
            programState->pwrlPosition.x = -64.8f;

        glm::mat4 powerlineModel = glm::mat4(1.0f);
        for (int i = 0; i < 10; ++i) {
            powerlineModel = glm::mat4(1.0f);
            powerlineModel = glm::translate(powerlineModel,programState->pwrlPosition + glm::vec3(16.2f * float(i), 0.0f, 0.0f));
            powerlineModel = glm::scale(powerlineModel, glm::vec3(programState->pwrlScale));
            powerlineModel = glm::rotate(powerlineModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            powerlineModel = glm::rotate(powerlineModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            ourShader.setMat4("model", powerlineModel);
            powerline.Draw(ourShader);
        }


        //render lampi
        if(programState->move)
            programState->lampPosition.x += speed * deltaTime;
        if (programState->lampPosition.x >= -60.0f)
            programState->lampPosition.x = -90.0f;

        glm::mat4 lampModel = glm::mat4(1.0f);
        for (int i = 0; i < 10; ++i) {
            lampModel = glm::mat4(1.0f);
            lampModel = glm::translate(lampModel,programState->lampPosition + glm::vec3(30.0f * float(i), 0.0f, 0.0f));
            lampModel = glm::scale(lampModel, glm::vec3(programState->lampScale));
            lampModel = glm::rotate(lampModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            ourShader.setMat4("model", lampModel);
            lamp.Draw(ourShader);
        }


        //spotlight za lampu
        std::vector<SpotLight> spotLights;
        for (int i = 0; i < 10; ++i) {
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
            programState->travaPosition.x = -60.0f;
        if (programState->trava2Position.x >= -40.0f)
            programState->trava2Position.x = -60.0f;

        glDisable(GL_CULL_FACE);
        glm::mat4 travaModel = glm::mat4(1.0f);
        //desno
        for (int i = 0; i < 16; ++i) {
            travaModel = glm::mat4(1.0f);
            travaModel = glm::translate(travaModel,programState->travaPosition + glm::vec3(9.0f * float(i), 0.0f, 0.0f));
            travaModel = glm::scale(travaModel, glm::vec3(programState->travaScale));
            travaModel = glm::rotate(travaModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            ourShader.setMat4("model", travaModel);
            trava.Draw(ourShader);
        }
        //levo
        for (int i = 0; i < 16; ++i) {
            travaModel = glm::mat4(1.0f);
            travaModel = glm::translate(travaModel,programState->trava2Position + glm::vec3(9.0f * float(i), 0.0f, 0.0f));
            travaModel = glm::scale(travaModel, glm::vec3(programState->travaScale));
            travaModel = glm::rotate(travaModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            ourShader.setMat4("model", travaModel);
            trava.Draw(ourShader);
        }


//        render kamencica
        if(programState->move)
            programState->trava2Position.x += speed * deltaTime;
        if (programState->travaPosition.x >= -40.0f)
            programState->travaPosition.x = -60.0f;

        glm::mat4 rocksModel = glm::mat4(1.0f);
        for (int i = 0; i < 10; ++i) {
            rocksModel = glm::mat4(1.0f);
            rocksModel = glm::translate(rocksModel,programState->rocksPosition + glm::vec3(10.0f * float(i), 0.0f, 0.0f));
            rocksModel = glm::scale(rocksModel, glm::vec3(programState->rocksScale));
            rocksModel = glm::rotate(rocksModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            ourShader.setMat4("model", rocksModel);
            rocks.Draw(ourShader);
        }
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


        // crta travu
        //ovaj deo koda je pozajmljen privremeno od kolege sa github naloga bodgans55
        glDisable(GL_CULL_FACE);
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.use();
        projection = glm::perspective(glm::radians(programState->camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = programState->camera.GetViewMatrix();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, planeTexture);
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_CULL_FACE);


        if (programState->ImGuiEnabled)
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
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &skyboxVAO);
    glDeleteBuffers(1, &planeVAO);

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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
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
    ImGui::Begin("Camera info");
    const Camera& c = programState->camera;
    ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
    ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
    ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
    ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
    ImGui::End();

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

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
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

unsigned int loadTexture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE
                                                                            : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
