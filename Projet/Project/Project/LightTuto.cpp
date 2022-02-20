#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <shader/shader_s.h>
#include <camera/camera.h>
#include <light/Light.h>
#include <mesh/mesh.h>
#include <model/model.h>

#include <iostream>
#include <list>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void setupLightSource(Shader lightSourceShader, unsigned int VAO);
void setupObject(Shader lightObjectShader, glm::vec3 lightPos, glm::vec3 cubePos);
void DrawCube(unsigned int VAO);
void DrawTerrain(Shader cubeShader, unsigned int VAO, int terrainSize, glm::vec3 rootPos);
void DrawWall(Shader ObjectShader, glm::vec3 wallPos, glm::mat4 rotation);
void Draw4Walls(Shader ObjectShader, unsigned int VAO, glm::vec3 wallPos);
void DrawGround(Shader ObjectShader, unsigned int VAO, glm::vec3 wallPos);
unsigned int genTextureFromPath(const char* texturePath);
void setupSkybox(Shader skyboxShader, unsigned int skyboxVAO, unsigned int cubemapTexture);
unsigned int loadCubemap(std::vector<std::string> faces);
void DrawEye(Shader eyeShader, Model eyeModel, glm::vec3 eyePos, int lampIndex);



// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(5.0f, 0.0f, -3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Lights
std::vector<Light> lightList;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
//glm::vec3 lightPos(1.2f, 1.8f, 2.0f);

//textures
unsigned int brickTexture;
unsigned int earthTexture;
unsigned int stoneTexture;
unsigned int woodTexture;


int main()
{
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("Objet.vert", "Objet.frag");
    Shader lightCubeShader("light_cubeV.vert", "light_cubeF.frag");
    Shader wallShader("Wall.vert", "Wall.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader modelShader("model.vert", "model.frag");

    brickTexture = genTextureFromPath("texture/brick.jpg");
    earthTexture = genTextureFromPath("texture/earth.jpg");
    stoneTexture = genTextureFromPath("texture/stone.jpg");
    woodTexture = genTextureFromPath("texture/wood.jpg");

    // Loading model
    Model eyeModel("resources/objets/eye/bigEye.obj");
    //Model secret("resources/objets/secret/secret.obj");
    //Model model1("resources/objets/backpack/backpack.obj");
    

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
    //Position            Normal
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    float wallVertices[] = {
        //Position           Normal              TextureCoord
        0.5f, 0.5f, 0.0f,    0.0f, 0.0f, -1.0f,   5.0f, 2.0f,
        0.5f, -0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   5.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
        0.5f, 0.5f, 0.0f,    0.0f, 0.0f, -1.0f,   5.0f, 2.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 2.0f
    };

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
    
    

    glm::vec3 cubePos = glm::vec3(0.0, -2.0, 0.0);
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, wallVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Wall setup
    unsigned int wallVAO;

    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glBindVertexArray(wallVAO);

    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glBindVertexArray(skyboxVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::vector<std::string> faces
    {
        "skybox/day/right.jpg",
        "skybox/day/left.jpg",
        "skybox/day/top.jpg",
        "skybox/day/bottom.jpg",
        "skybox/day/front.jpg",
        "skybox/day/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    std::vector<std::string> facesNight1
    {
        "skybox/nightskybox1/right.jpg",
        "skybox/nightskybox1/left.jpg",
        "skybox/nightskybox1/top.jpg",
        "skybox/nightskybox1/bottom.jpg",
        "skybox/nightskybox1/front.jpg",
        "skybox/nightskybox1/back.jpg"
    };
    unsigned int cubemapTextureNight1 = loadCubemap(facesNight1);

    std::vector<std::string> facesNight2
    {
        "skybox/nightskybox2/right.jpg",
        "skybox/nightskybox2/left.jpg",
        "skybox/nightskybox2/top.jpg",
        "skybox/nightskybox2/bottom.jpg",
        "skybox/nightskybox2/front.jpg",
        "skybox/nightskybox2/back.jpg"
    };
    unsigned int cubemapTextureNight2 = loadCubemap(facesNight2);

    
    //Light MainLamp = Light::Light(glm::vec3(7.5, 1.0, -2.5), glm::vec3(1.0,1.0,1.0), 3, 2);
    //lightList.push_back(MainLamp);

    Light RedLight1 = Light::Light(glm::vec3(-0.5, 1.2, 0.5), glm::vec3(0.2, 0.6, 0.1), 1, 3);
    lightList.push_back(RedLight1);

    Light RedLight2 = Light::Light(glm::vec3(-0.5, 1.2, -14.5), glm::vec3(0.2, 0.6, 0.1), 1, 3);
    lightList.push_back(RedLight2);

    Light RedLight3 = Light::Light(glm::vec3(14.5, 1.2, -14.5), glm::vec3(0.2, 0.6, 0.1), 1, 3);
    lightList.push_back(RedLight3);

    Light RedLight4 = Light::Light(glm::vec3(14.5, 1.2, 0.5), glm::vec3(0.2, 0.6, 0.1), 1, 3);
    lightList.push_back(RedLight4);

    Light eyeLamp = Light::Light(glm::vec3(7.5, -0.3, -7), glm::vec3(1.0, 0.0, 0.0), 10, 2, glm::vec3(0.0,0.0,-1.0), 10, 7, true);
    lightList.push_back(eyeLamp);

    Light SpotLight = Light::Light(glm::vec3(7.5, 4.0f, -7.0), glm::vec3(1.0, 1.0, 1.0), 2.0, 5, glm::vec3(0.0, -1.0, -0.4), 40.0f, 50.0f, true);
    lightList.push_back(SpotLight);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the lamp object
        setupLightSource(lightCubeShader, lightCubeVAO);

        

        // Draw Terrain
        //DrawTerrain(lightingShader, cubeVAO, 15, cubePos);
        /*
        wallShader.use();
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.5f, -0.5f, -7.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::rotate(model, 30 * glm::radians((float)glfwGetTime()), glm::vec3(0.0, 1.0, 0.0));
        wallShader.setMat4("model", model);
        eyeModel.Draw(wallShader);
        */

        DrawEye(wallShader, eyeModel, glm::vec3(7.5f, -0.5f, -7.0f), 4);

        // Draw Walls
        glBindTexture(GL_TEXTURE_2D, 1);
        Draw4Walls(wallShader, wallVAO, cubePos);

        // Draw Ground
        glBindTexture(GL_TEXTURE_2D, 4);
        DrawGround(wallShader, wallVAO, cubePos);   

        // Draw skybox
        setupSkybox(skyboxShader, skyboxVAO, cubemapTextureNight2);

        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &wallVAO);


    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void setupLightSource(Shader lightSourceShader, unsigned int VAO) {
    lightSourceShader.use();
    lightList[5].setStrength(abs(sin(glfwGetTime())));
    for (int i = 0; i < lightList.size(); i++) {
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        lightSourceShader.setVec3("lightCubeColor", lightList[i].lightColor);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightList[i].lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));        
        DrawCube(VAO);
    }    
}


void setupObject(Shader ObjectShader, glm::vec3 lightPos, glm::vec3 cubePos) {
    ObjectShader.use();
    glUniform3f(glGetUniformLocation(ObjectShader.ID, "objectColor"), 0.33f, 0.01f, 0.45f);
    glUniform3f(glGetUniformLocation(ObjectShader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3fv(glGetUniformLocation(ObjectShader.ID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(ObjectShader.ID, "viewPos"), 1, glm::value_ptr(camera.Position));
    
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(ObjectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(ObjectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cubePos);
    glUniformMatrix4fv(glGetUniformLocation(ObjectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
}

void DrawCube(unsigned int VAO) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void DrawTerrain(Shader cubeShader, unsigned int VAO, int terrainSize, glm::vec3 rootPos) {

    for (unsigned int i = 0; i < terrainSize; i++) {
        for (unsigned int j = 0; j < terrainSize; j++) {
            glm::vec3 pos = glm::vec3(j + rootPos.x, rootPos.y, (-1.0 * i) + rootPos.z);
            setupObject(cubeShader, glm::vec3(2.0, 1.0, 0.0), pos);
            DrawCube(VAO);
        }
    }
}

void DrawWall(Shader ObjectShader, glm::vec3 wallPos, glm::mat4 rotation) {
    std::string number;
    for (int i = 0; i < lightList.size(); i++) {
        number = std::to_string(i);
        ObjectShader.use();
        glUniform3fv(glGetUniformLocation(ObjectShader.ID, ("lightColor[" + number + "]").c_str()), 1, glm::value_ptr(lightList[i].lightColor));
        glUniform3fv(glGetUniformLocation(ObjectShader.ID, ("lightPos[" + number + "]").c_str()), 1, glm::value_ptr(lightList[i].lightPos));
        glUniform3fv(glGetUniformLocation(ObjectShader.ID, "viewPos"), 1, glm::value_ptr(camera.Position));
        glUniform1f(glGetUniformLocation(ObjectShader.ID, ("strength[" + number + "]").c_str()), lightList[i].strength);
        glUniform1f(glGetUniformLocation(ObjectShader.ID, ("range[" + number + "]").c_str()), lightList[i].range);
        glUniform3fv(glGetUniformLocation(ObjectShader.ID, ("spotDir[" + number + "]").c_str()), 1, glm::value_ptr(lightList[i].spotDir));
        glUniform1f(glGetUniformLocation(ObjectShader.ID, ("cutOff[" + number + "]").c_str()), glm::cos(glm::radians(lightList[i].cutOff)));
        glUniform1f(glGetUniformLocation(ObjectShader.ID, ("outerCutOff[" + number + "]").c_str()), glm::cos(glm::radians(lightList[i].outerCutOff)));
        ObjectShader.setBool(("isSpot[" + number + "]").c_str(), lightList[i].isSpot);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(ObjectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ObjectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, wallPos);
        model = glm::scale(model, glm::vec3(15, 3, 15));
        model = model * rotation;
        glUniformMatrix4fv(glGetUniformLocation(ObjectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    }    
}

void Draw4Walls(Shader ObjectShader, unsigned int VAO, glm::vec3 wallPos) {
    glm::vec3 pos = wallPos;
    
    //Wall 1
    glm::vec3 pos1 = glm::vec3(wallPos.x + 7.0f, wallPos.y + 2.0f, wallPos.z + 0.5f);
    DrawWall(ObjectShader, pos1, glm::mat4(1.0f));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //Wall 2
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::vec3 pos2 = glm::vec3(wallPos.x -0.5f, wallPos.y + 2.0f, wallPos.z - 7.0f);
    DrawWall(ObjectShader, pos2, rotation);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //Wall 3
    glm::vec3 pos3 = glm::vec3(wallPos.x + 7.0f, wallPos.y + 2.0f, wallPos.z -14.5f);
    rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    DrawWall(ObjectShader, pos3, rotation);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //Wall 4
    rotation = glm::rotate(rotation, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::vec3 pos4 = glm::vec3(wallPos.x +14.5f, wallPos.y + 2.0f, wallPos.z - 7.0f);
    DrawWall(ObjectShader, pos4, rotation);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
        
}

void DrawGround(Shader ObjectShader, unsigned int VAO, glm::vec3 wallPos) {
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    //rotation = glm::rotate(rotation, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::vec3 pos5 = glm::vec3(wallPos.x + 7.0f, wallPos.y + 0.5, wallPos.z - 7.0f);
    DrawWall(ObjectShader, pos5, rotation);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


unsigned int genTextureFromPath(const char* texturePath) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

void setupSkybox(Shader skyboxShader, unsigned int skyboxVAO, unsigned int cubemapTexture) {
    glDepthFunc(GL_LEQUAL);
    skyboxShader.use();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failde to load at path: " << faces[i] << std::endl;
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

void DrawEye(Shader eyeShader, Model eyeModel, glm::vec3 eyePos, int lampIndex) {
    eyeShader.use();
    // render the loaded model
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    eyeShader.setMat4("projection", projection);
    eyeShader.setMat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 posCam = camera.getPosition();
    float angle;
    model = glm::translate(model, eyePos);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    angle = atan2f(eyePos.x - posCam.x, eyePos.z - posCam.z) + glm::radians(180.0f);
    
    lightList[lampIndex].setAngle(glm::vec3(posCam.x - eyePos.x, 0.0, posCam.z - eyePos.z));
    model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(eyeShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    eyeModel.Draw(eyeShader);
}