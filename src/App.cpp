#include "App.hpp"
#include <iostream>
#include <algorithm>
#include <imgui.h>

App::App() : totalTime(0.0f) {}

void App::checkGLError(const std::string& label) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "[GL Error] " << label << " - Code: " << err << std::endl;
    }
}

void App::init(const glimac::FilePath& applicationPath) {
    // --- 1. CHARGEMENT DE LA POLICE IMGUI ---
    ImGuiIO& io = ImGui::GetIO();
    static ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    static const ImWchar chessRanges[] = { 0x2654, 0x265F, 0 };
    builder.AddRanges(chessRanges);
    builder.BuildRanges(&ranges);

    // Construction du chemin robuste vers la police
    std::string fontPath = applicationPath.dirPath() + "../../assets/FreeSerif/FreeSerif.ttf";
    chessFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 40.0f, nullptr, ranges.Data);

    if (!chessFont) {
        std::cerr << "[WARNING] Impossible de charger la police : " << fontPath << std::endl;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    try {
        std::string vsPath = applicationPath.dirPath() + "../../src/shaders/3d.vs.glsl";
        std::string fsPath = applicationPath.dirPath() + "../../src/shaders/3d.fs.glsl";
        
        program = std::make_unique<glimac::Program>(glimac::loadProgram(glimac::FilePath(vsPath), glimac::FilePath(fsPath)));
        uMVPLocation = glGetUniformLocation(program->getGLId(), "uMVP");
        uColorLocation = glGetUniformLocation(program->getGLId(), "uColor");
        uLightingModeLocation = glGetUniformLocation(program->getGLId(), "uLightingMode");
        uTimeLocation = glGetUniformLocation(program->getGLId(), "uTime");
        
        renderer = std::make_unique<Renderer3D>();
        lastTime = std::chrono::steady_clock::now();
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception lors de l'initialisation: " << e.what() << std::endl;
        throw;
    }
}

void App::handleCameraInput() {
    ImGuiIO& io = ImGui::GetIO();
    // On ne bouge la caméra que si la souris n'est PAS sur une fenêtre ImGui
    if (!io.WantCaptureMouse) {
        // Clic gauche + Drag pour tourner
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            camera.rotateLeft(io.MouseDelta.x * 0.5f);
            camera.rotateUp(io.MouseDelta.y * 0.5f);
        }
        // Molette pour zoomer
        if (io.MouseWheel != 0.0f) {
            camera.moveFront(io.MouseWheel * 1.0f);
        }
    }
    // Touche C pour changer de mode caméra
    if (ImGui::IsKeyPressed(ImGuiKey_C)) {
        if (camera.getMode() == TRACKBALL) {
            camera.setMode(PIECE_VIEW);
        } else {
            camera.setMode(TRACKBALL);
        }
    }
}

void App::drawBoard(const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix) {
    if (!program || program->getGLId() <= 0) return;

    program->use();
    checkGLError("program.use()");

    int lightingMode = (chessGame.getTurn() == Color::White) ? 0 : 1;
    glUniform1i(uLightingModeLocation, lightingMode);
    glUniform1f(uTimeLocation, totalTime);

    const Board& board = chessGame.getBoard();
    const auto& pathSquares = chessGame.getPathSquares();
    bool pathAnimating = chessGame.isPathAnimating();
    float pathTime = chessGame.getPathTime();
    float pathDuration = chessGame.getPathDuration();

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            bool isLightSquare = ((x + y) % 2 == 0);
            glm::vec4 squareColor = isLightSquare ?
                glm::vec4(0.98f, 0.76f, 0.45f, 1.0f) :
                glm::vec4(0.58f, 0.32f, 0.23f, 1.0f);

            float elevation = 0.0f;
            if (pathAnimating) {
                for (size_t i = 0; i < pathSquares.size(); ++i) {
                    if (pathSquares[i].x == x && pathSquares[i].y == y) {
                        float phase = std::clamp((pathTime / pathDuration) * (static_cast<float>(pathSquares.size()) + 1.0f) - static_cast<float>(i), 0.0f, 1.0f);
                        elevation = std::sin(phase * glm::pi<float>()) * 0.25f;
                        squareColor = glm::mix(squareColor, glm::vec4(0.2f, 0.8f, 1.0f, 1.0f), 0.4f);
                    }
                }
            }

            glUniform4fv(uColorLocation, 1, glm::value_ptr(squareColor));
            glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.01f + elevation, y));
            
            Piece* piece = board.getPiece({x, y});
            if (piece) {
                glm::vec4 pieceColorVec = (piece->getColor() == Color::White) ?
                    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
                glUniform4fv(uColorLocation, 1, glm::value_ptr(pieceColorVec));
                ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.8f, 1.5f, 0.8f));
            } else {
                ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
            }

            glm::mat4 mvp = ProjMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(mvp));
            renderer->drawCube();
        }
    }

    // Bordures
    glm::vec4 borderColor = glm::vec4(0.35f, 0.20f, 0.10f, 1.0f);
    glUniform4fv(uColorLocation, 1, glm::value_ptr(borderColor));
    auto drawBorder = [&](glm::vec3 pos, glm::vec3 scale) {
        glm::mat4 M = glm::scale(glm::translate(glm::mat4(1.0f), pos), scale);
        glm::mat4 mvp = ProjMatrix * ViewMatrix * M;
        glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        renderer->drawCube();
    };
    drawBorder(glm::vec3(3.5f, 0.05f, -0.5f), glm::vec3(8.5f, 0.12f, 0.5f));
    drawBorder(glm::vec3(3.5f, 0.05f, 8.5f), glm::vec3(8.5f, 0.12f, 0.5f));
    drawBorder(glm::vec3(-0.5f, 0.05f, 3.5f), glm::vec3(0.5f, 0.12f, 8.5f));
    drawBorder(glm::vec3(8.5f, 0.05f, 3.5f), glm::vec3(0.5f, 0.12f, 8.5f));
}

void App::render() {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - lastTime).count();
    totalTime += dt;
    lastTime = now;

    chessGame.updatePathAnimation(dt);
    handleCameraInput(); // Gestion de la caméra

    // Si en mode pièce et une pièce sélectionnée, positionner la caméra
    if (camera.getMode() == PIECE_VIEW) {
        Piece* selected = chessGame.getSelectedPiece();
        if (selected) {
            glm::vec3 basePos;
            if (chessGame.isPathAnimating()) {
                Position start = chessGame.getPathStart();
                Position target = chessGame.getPathTarget();
                float t = chessGame.getPathTime() / chessGame.getPathDuration();
                basePos = glm::vec3(start.x, 0.0f, start.y) + glm::vec3(target.x - start.x, 0.0f, target.y - start.y) * t;
            } else {
                Position pos = selected->getPos();
                basePos = glm::vec3(pos.x, 0.0f, pos.y);
            }
            // Position au centre au-dessus de la pièce (sommet)
            camera.setPiecePosition(basePos + glm::vec3(0.5f, 0.8f, 0.5f));
        }
    }

    Color currentTurn = chessGame.getTurn();
    if (currentTurn == Color::White) glClearColor(0.8f, 0.9f, 1.0f, 1.0f);
    else glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(45.f), 1.0f, 0.1f, 100.f);
    glm::mat4 ViewMatrix = camera.getViewMatrix(); // Utilisation de la nouvelle caméra

    drawBoard(ViewMatrix, ProjMatrix);

    // --- UI ImGui ---
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("Chess Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Tour : %s", currentTurn == Color::White ? "Blancs" : "Noirs");
    if (chessFont) ImGui::PushFont(chessFont);
    
    view.draw(chessGame);
    
    if (chessFont) ImGui::PopFont();

    ImGui::End();
    
    ImGui::SetNextWindowPos(ImVec2(420, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("3D View Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Clic gauche + glisser pour tourner");
    ImGui::Text("Molette pour zoomer");
    if (ImGui::Button("Changer Mode Camera")) {
        if (camera.getMode() == TRACKBALL) {
            camera.setMode(PIECE_VIEW);
        } else {
            camera.setMode(TRACKBALL);
        }
    }
    ImGui::Text("Mode actuel: %s", camera.getMode() == TRACKBALL ? "Trackball" : "Piece");
    ImGui::End();
}