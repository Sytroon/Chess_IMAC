#include <iostream>
#include "chessView.hpp"
#include "game.hpp"
#include "quick_imgui/quick_imgui.hpp"
#include "Renderer3D.hpp"
#include "glimac/Program.hpp"



// On déclare le pointeur de police en dehors pour y avoir accès dans la boucle
ImFont* chessFont = nullptr;

void checkGLError(const std::string& label) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "[GL Error] " << label << " - Code: " << err << std::endl;
    }
}

int main(int argc, char** argv)
{
    Game      chessGame;
    ChessView view;
    glimac::FilePath applicationPath(argv[0]);
    
    // Variables globales de rendu (initialisées après contexte OpenGL)
    std::unique_ptr<Renderer3D> renderer;
    std::unique_ptr<glimac::Program> program;
    GLint uMVPLocation = -1;

    quick_imgui::loop("Chess Project 3D", {
        .init = [&]() {
            // --- INITIALISATION OPENGL 3D ---
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            
            // --- CHARGEMENT DES SHADERS ---
            try {
                std::string vsPath = applicationPath.dirPath() + "../../src/shaders/3d.vs.glsl";
                std::string fsPath = applicationPath.dirPath() + "../../src/shaders/3d.fs.glsl";
                
                program = std::make_unique<glimac::Program>(glimac::loadProgram(glimac::FilePath(vsPath), glimac::FilePath(fsPath)));
                uMVPLocation = glGetUniformLocation(program->getGLId(), "uMVP");
                
                // --- INITIALISER LE RENDERER ---
                renderer = std::make_unique<Renderer3D>();
                
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception lors de l'initialisation: " << e.what() << std::endl;
                throw;
            }
        },
        
        .loop = [&]() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            checkGLError("glClear");

            // Calcul des matrices de vue
            glm::mat4 ProjMatrix = glm::perspective(glm::radians(45.f), 1.0f, 0.1f, 100.f);
            glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(4, 10, 10), glm::vec3(4, 0, 4), glm::vec3(0, 1, 0));

            if (program && program->getGLId() > 0) {
                program->use();
                checkGLError("program.use()");

                // Dessiner le plateau en 3D
                const Board& board = chessGame.getBoard();
                for (int x = 0; x < 8; ++x) {
                    for (int y = 0; y < 8; ++y) {
                        glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, y));
                        
                        if (board.getPiece({x, y})) {
                            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.8, 1.5, 0.8));
                        } else {
                            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0, 0.1, 1.0));
                        }

                        glm::mat4 mvp = ProjMatrix * ViewMatrix * ModelMatrix;
                        glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(mvp));
                        
                        if (renderer) {
                            renderer->drawCube();
                        }
                    }
                }
            }

            // UI ImGui
            ImGui::Begin("Chess Controls");
            ImGui::Text("Tour : %s", chessGame.getTurn() == Color::White ? "Blancs" : "Noirs");
            view.draw(chessGame);
            ImGui::End();
        }
    });

    return 0;
}