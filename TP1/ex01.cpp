#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/Sphere.hpp>
#include <glimac/glm.hpp>
#include <glimac/Program.hpp> 
#include <glimac/FilePath.hpp>
#include <glimac/TrackballCamera.hpp>

// --- STRUCTURE DE DONNÉES ---
// On regroupe les ressources pour éviter de passer 15 arguments aux fonctions
struct SceneResources {
    glimac::Program program;
    glimac::Sphere sphere;
    GLuint vao, vbo;
    GLuint uMVPMatrixLoc, uMVMatrixLoc, uNormalMatrixLoc;

    SceneResources() : sphere(1, 32, 16) {}
};

// --- VARIABLES D'ÉTAT GLOBALES ---
int window_width = 800;
int window_height = 800;
glimac::TrackballCamera camera;
bool isDragging = false;
double lastX = 0.0, lastY = 0.0;

// --- CALLBACKS (Identiques au précédent) ---
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) { isDragging = true; glfwGetCursorPos(window, &lastX, &lastY); }
        else if (action == GLFW_RELEASE) { isDragging = false; }
    }
}
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { camera.moveFront(-yoffset); }
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging) {
        camera.rotateLeft((xpos - lastX) * 0.1f);
        camera.rotateUp((ypos - lastY) * 0.1f);
        lastX = xpos; lastY = ypos;
    }
}
static void size_callback(GLFWwindow*, int width, int height) { window_width = width; window_height = height; glViewport(0, 0, width, height); }

// --- FONCTIONS D'ENCAPSULATION ---

// 1. Initialisation complète de l'environnement
GLFWwindow* setupApp() {
    if (!glfwInit()) return nullptr;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP1 - Encapsulé", nullptr, nullptr);
    if (!window) { glfwTerminate(); return nullptr; }
    
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, size_callback);
    
    glEnable(GL_DEPTH_TEST);
    return window;
}

// 2. Chargement et préparation des ressources de la scène
void loadSceneResources(SceneResources& res, const char* execPath) {
    glimac::FilePath applicationPath(execPath);
    res.program = loadProgram(applicationPath.dirPath() + "TP1/shaders/3D.vs.glsl",
                                applicationPath.dirPath() + "TP1/shaders/normals.fs.glsl");
    
    res.uMVPMatrixLoc = glGetUniformLocation(res.program.getGLId(), "uMVPMatrix");
    res.uMVMatrixLoc = glGetUniformLocation(res.program.getGLId(), "uMVMatrix");
    res.uNormalMatrixLoc = glGetUniformLocation(res.program.getGLId(), "uNormalMatrix");

    glGenBuffers(1, &res.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, res.vbo);
    glBufferData(GL_ARRAY_BUFFER, res.sphere.getVertexCount() * sizeof(glimac::ShapeVertex), res.sphere.getDataPointer(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &res.vao);
    glBindVertexArray(res.vao);
    glBindBuffer(GL_ARRAY_BUFFER, res.vbo);
    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, position));
    glEnableVertexAttribArray(1); // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, normal));
    glEnableVertexAttribArray(2); // TexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, texCoords));
    
    glBindVertexArray(0);
}

// 3. Dessin d'un objet spécifique (utilitaire interne)
void drawObject(const SceneResources& res, const glm::mat4& ModelMatrix, const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix) {
    glm::mat4 MVMatrix = ViewMatrix * ModelMatrix;
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
    glm::mat4 MVP = ProjMatrix * MVMatrix;

    glUniformMatrix4fv(res.uMVPMatrixLoc, 1, GL_FALSE, glm::value_ptr(MVP));
    glUniformMatrix4fv(res.uMVMatrixLoc, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(res.uNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

    glBindVertexArray(res.vao);
    glDrawArrays(GL_TRIANGLES, 0, res.sphere.getVertexCount());
}

// 4. La boucle de rendu complète
void runMainLoop(GLFWwindow* window, SceneResources& res) {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        res.program.use();

        float time = glfwGetTime();
        glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), (float)window_width / window_height, 0.1f, 100.f);
        glm::mat4 ViewMatrix = camera.getViewMatrix();

        // Dessin Terre
        drawObject(res, glm::mat4(1.0f), ViewMatrix, ProjMatrix);

        // Dessin Lune
        glm::mat4 ModelMatrixMoon = glm::mat4(1.0f);
        ModelMatrixMoon = glm::translate(ModelMatrixMoon, glm::vec3(0, 0, -2));
        ModelMatrixMoon = glm::rotate(ModelMatrixMoon, time, glm::vec3(0, 0, 1));
        ModelMatrixMoon = glm::translate(ModelMatrixMoon, glm::vec3(-2, 0, 0));
        ModelMatrixMoon = glm::scale(ModelMatrixMoon, glm::vec3(0.2, 0.2, 0.2));
        drawObject(res, ModelMatrixMoon, ViewMatrix, ProjMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// 5. Nettoyage
void cleanup(SceneResources& res) {
    glDeleteBuffers(1, &res.vbo);
    glDeleteVertexArrays(1, &res.vao);
    glfwTerminate();
}

// --- MAIN MINIMALISTE ---
int main(int argc, char* argv[]) {
    // 1. Initialisation
    GLFWwindow* window = setupApp();
    if (!window) return -1;

    // 2. Ressources
    SceneResources res;
    loadSceneResources(res, argv[0]);

    // 3. Boucle principale
    runMainLoop(window, res);

    // 4. Nettoyage
    cleanup(res);

    return 0;
}