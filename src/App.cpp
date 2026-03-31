#include "App.hpp"
#include <iostream>
#include <algorithm>
#include <imgui.h>

namespace {
constexpr int   kBoardCellCount  = 8;
constexpr float kSquareSize      = 1.0f;
constexpr float kBoardMinCoord   = -0.5f * kSquareSize;
constexpr float kBoardMaxCoord   = (static_cast<float>(kBoardCellCount) - 0.5f) * kSquareSize;
constexpr float kBoardCenterCoord = (kBoardMinCoord + kBoardMaxCoord) * 0.5f;
constexpr float kBorderThickness = 0.5f;

Renderer3D::PieceMeshType meshTypeForPiece(const Piece& piece) {
    if (dynamic_cast<const King*>(&piece) != nullptr) {
        return Renderer3D::PieceMeshType::King;
    }
    if (dynamic_cast<const Queen*>(&piece) != nullptr) {
        return Renderer3D::PieceMeshType::Queen;
    }
    if (dynamic_cast<const Bishop*>(&piece) != nullptr) {
        return Renderer3D::PieceMeshType::Bishop;
    }
    if (dynamic_cast<const Knight*>(&piece) != nullptr) {
        return Renderer3D::PieceMeshType::Knight;
    }
    if (dynamic_cast<const Rook*>(&piece) != nullptr) {
        return Renderer3D::PieceMeshType::Rook;
    }
    return Renderer3D::PieceMeshType::Pawn;
}

glm::vec3 scaleForPiece(const Piece& piece) {
    constexpr float globalScale = 2.0f; // scalaire pour taille globale

    if (dynamic_cast<const Pawn*>(&piece) != nullptr) {
        return glm::vec3(0.6f, 1.2f, 0.6f) * globalScale;
    }
    if (dynamic_cast<const Rook*>(&piece) != nullptr) {
        return glm::vec3(0.72f) * globalScale;
    }
    if (dynamic_cast<const Knight*>(&piece) != nullptr) {
        return glm::vec3(0.78f) * globalScale;
    }
    if (dynamic_cast<const Bishop*>(&piece) != nullptr) {
        return glm::vec3(0.82f) * globalScale;
    }
    if (dynamic_cast<const Queen*>(&piece) != nullptr) {
        return glm::vec3(0.88f) * globalScale;
    }
    return glm::vec3(0.96f) * globalScale;
}

struct PieceOrientation {
    float pitch = 0.f; // Rotation autour de X : pencher vers l'avant / l'arrière
    float yaw   = 0.f; // Rotation autour de Y : tourner gauche / droite
    float roll  = 0.f; // Rotation autour de Z : incliner sur le côté
};

PieceOrientation orientationForPiece(const Piece& piece) {
    return {
        0.f,                                                // pitch : avant / arrière
        (piece.getColor() == Color::White) ? 90.f : -90.f, // yaw   : direction selon la couleur
        0.f                                                 // roll  : inclinaison latérale
    };
}

glm::mat4 facingRotationForPiece(const Piece& piece) {
    const PieceOrientation orientation = orientationForPiece(piece);

    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(orientation.pitch), glm::vec3(1.f, 0.f, 0.f));
    rotation = glm::rotate(rotation, glm::radians(orientation.yaw), glm::vec3(0.f, 1.f, 0.f));
    rotation = glm::rotate(rotation, glm::radians(orientation.roll), glm::vec3(0.f, 0.f, 1.f));
    return rotation;
}

glm::vec3 boardCenterForPosition(const Position& pos, float height = 0.0f) {
    const float worldX = kBoardMinCoord + 0.5f * kSquareSize + static_cast<float>(pos.x) * kSquareSize;
    const float worldZ = kBoardMinCoord + 0.5f * kSquareSize + static_cast<float>(pos.y) * kSquareSize;
    return glm::vec3(worldX, height, worldZ);
}

glm::vec3 localOffsetForPiece(const Piece& piece) {
    // Décalage local du modèle dans sa case.
    // X = gauche/droite, Y = hauteur, Z = avant/arrière.
    if (dynamic_cast<const Pawn*>(&piece) != nullptr) {
        return glm::vec3(0.55f, 0.00f, -0.35f);
    }
    if (dynamic_cast<const Knight*>(&piece) != nullptr) {
        return glm::vec3(0.4f, 0.00f, -0.65f); 
    }
    if (dynamic_cast<const Bishop*>(&piece) != nullptr) {
        return glm::vec3(0.25f, 0.00f, -0.75f);
    }
    if (dynamic_cast<const Rook*>(&piece) != nullptr) {
        return glm::vec3(0.55f, 0.00f, -0.65f);
    }
    if (dynamic_cast<const Queen*>(&piece) != nullptr) {
        return glm::vec3(0.00f, 0.00f, -0.75f);
    }
    return glm::vec3(0.00f, 0.00f, -0.75f); // Roi
}
} // namespace

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
        
        std::string piecesRoot = applicationPath.dirPath() + "../../assets";
        renderer = std::make_unique<Renderer3D>(piecesRoot);
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
    if (!program || program->getGLId() <= 0 || !renderer) return;

    constexpr float kSquareHeight = 0.12f;
    constexpr float kBorderHeight = 0.18f;

    program->use();
    checkGLError("program.use()");

    const int lightingMode = (chessGame.getTurn() == Color::White) ? 0 : 1;
    glUniform1i(uLightingModeLocation, lightingMode);
    glUniform1f(uTimeLocation, totalTime);

    const Board& board = chessGame.getBoard();
    const auto&  pathSquares = chessGame.getPathSquares();
    const bool   pathAnimating = chessGame.isPathAnimating();
    const float  pathTime = chessGame.getPathTime();
    const float  pathDuration = chessGame.getPathDuration();

    for (int boardX = 0; boardX < kBoardCellCount; ++boardX) {
        for (int boardZ = 0; boardZ < kBoardCellCount; ++boardZ) {
            const bool isLightSquare = ((boardX + boardZ) % 2 == 0);
            glm::vec4  squareColor = isLightSquare
                ? glm::vec4(0.98f, 0.76f, 0.45f, 1.0f)
                : glm::vec4(0.58f, 0.32f, 0.23f, 1.0f);

            float elevation = 0.0f;
            if (pathAnimating) {
                for (size_t i = 0; i < pathSquares.size(); ++i) {
                    if (pathSquares[i].x == boardX && pathSquares[i].y == boardZ) {
                        const float phase = std::clamp(
                            (pathTime / pathDuration) * (static_cast<float>(pathSquares.size()) + 1.0f) - static_cast<float>(i),
                            0.0f,
                            1.0f
                        );
                        elevation = std::sin(phase * glm::pi<float>()) * 0.25f;
                        squareColor = glm::mix(squareColor, glm::vec4(0.2f, 0.8f, 1.0f, 1.0f), 0.4f);
                        break;
                    }
                }
            }

            glUniform4fv(uColorLocation, 1, glm::value_ptr(squareColor));
            const glm::vec3 squareCenter = boardCenterForPosition({boardX, boardZ}, elevation + kSquareHeight * 0.5f);
            glm::mat4 squareModel = glm::translate(glm::mat4(1.0f), squareCenter);
            squareModel = glm::scale(squareModel, glm::vec3(kSquareSize, kSquareHeight, kSquareSize));

            glm::mat4 squareMvp = ProjMatrix * ViewMatrix * squareModel;
            glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(squareMvp));
            renderer->drawCube();

            Piece* piece = board.getPiece({boardX, boardZ});
            if (piece == nullptr) {
                continue;
            }

            const glm::vec4 pieceColor = (piece->getColor() == Color::White)
                ? glm::vec4(0.92f, 0.92f, 0.92f, 1.0f)
                : glm::vec4(0.18f, 0.18f, 0.18f, 1.0f);
            glUniform4fv(uColorLocation, 1, glm::value_ptr(pieceColor));

            const auto pieceType = meshTypeForPiece(*piece);
            const auto pieceScale = scaleForPiece(*piece);
            const float verticalOffset = 0.0f;
            const Position boardPos{boardX, boardZ};
            const glm::vec3 squareCenterForPiece = boardCenterForPosition(boardPos, elevation + kSquareHeight + verticalOffset);

            glm::mat4 pieceModel = glm::translate(glm::mat4(1.0f), squareCenterForPiece);
            pieceModel = pieceModel * facingRotationForPiece(*piece);
            pieceModel = glm::translate(pieceModel, localOffsetForPiece(*piece));
            pieceModel = glm::scale(pieceModel, pieceScale);

            glm::mat4 pieceMvp = ProjMatrix * ViewMatrix * pieceModel;
            glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(pieceMvp));
            renderer->drawPiece(pieceType);
        }
    }

    glm::vec4 borderColor = glm::vec4(0.35f, 0.20f, 0.10f, 1.0f);
    glUniform4fv(uColorLocation, 1, glm::value_ptr(borderColor));
    auto drawBorder = [&](const glm::vec3& pos, const glm::vec3& scale) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, scale);
        const glm::mat4 mvp = ProjMatrix * ViewMatrix * model;
        glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        renderer->drawCube();
    };

    const float halfBorderHeight = kBorderHeight * 0.5f;
    const float outerMin = kBoardMinCoord - kBorderThickness * 0.5f;
    const float outerMax = kBoardMaxCoord + kBorderThickness * 0.5f;
    const float frameSpan = (kBoardMaxCoord - kBoardMinCoord) + kBorderThickness;

    drawBorder(glm::vec3(kBoardCenterCoord, halfBorderHeight, outerMin), glm::vec3(frameSpan, kBorderHeight, kBorderThickness));
    drawBorder(glm::vec3(kBoardCenterCoord, halfBorderHeight, outerMax), glm::vec3(frameSpan, kBorderHeight, kBorderThickness));
    drawBorder(glm::vec3(outerMin, halfBorderHeight, kBoardCenterCoord), glm::vec3(kBorderThickness, kBorderHeight, frameSpan));
    drawBorder(glm::vec3(outerMax, halfBorderHeight, kBoardCenterCoord), glm::vec3(kBorderThickness, kBorderHeight, frameSpan));
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
                basePos = glm::mix(boardCenterForPosition(start), boardCenterForPosition(target), t);
            } else {
                Position pos = selected->getPos();
                basePos = boardCenterForPosition(pos);
            }
            // Position au centre de la pièce pour le mode de vue rapproché
            camera.setPiecePosition(basePos + glm::vec3(0.0f, 0.9f, 0.0f));
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