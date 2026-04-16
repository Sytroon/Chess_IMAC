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

// Fonction de Raycasting pour trouver la case survolée par la souris
bool getHoveredSquare(
    float mouseX, float mouseY, 
    float screenWidth, float screenHeight, 
    const glm::mat4& viewMatrix, const glm::mat4& projMatrix, 
    Position& outHoveredPos) 
{
    // Coordonnées normalisées (-1 à 1)
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight; // Y inversé
    
    glm::vec4 ray_clip(x, y, -1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(projMatrix) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    
    // Direction du rayon dans l'espace monde
    glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(viewMatrix) * ray_eye));
    
    // Position de la caméra (extraite de la matrice de vue)
    glm::vec3 cameraPos = glm::vec3(glm::inverse(viewMatrix)[3]);
    
    float boardHeight = 0.30f; // kSquareHeight
    
    // Si on regarde vers le haut ou parallèle, on ne touche pas le plateau
    if (std::abs(ray_wor.y) < 0.001f || ray_wor.y > 0.0f) return false;
    
    // Calcul de l'intersection avec le plan du plateau
    float t = (boardHeight - cameraPos.y) / ray_wor.y;
    glm::vec3 intersection = cameraPos + ray_wor * t;
    
    int rawX = static_cast<int>(std::floor((intersection.x - kBoardMinCoord) / kSquareSize));
    int rawZ = static_cast<int>(std::floor((intersection.z - kBoardMinCoord) / kSquareSize));
    
    // On conserve le X brut, mais on garde l'inversion sur Z
    int gridX = rawX;
    int gridZ = (kBoardCellCount - 1) - rawZ;
    
    if (gridX >= 0 && gridX < kBoardCellCount && gridZ >= 0 && gridZ < kBoardCellCount) {
        outHoveredPos = {gridX, gridZ};
        return true;
    }
    return false;
}

Renderer3D::PieceMeshType meshTypeForPiece(const Piece& piece) {
    if (dynamic_cast<const King*>(&piece) != nullptr) return Renderer3D::PieceMeshType::King;
    if (dynamic_cast<const Queen*>(&piece) != nullptr) return Renderer3D::PieceMeshType::Queen;
    if (dynamic_cast<const Bishop*>(&piece) != nullptr) return Renderer3D::PieceMeshType::Bishop;
    if (dynamic_cast<const Knight*>(&piece) != nullptr) return Renderer3D::PieceMeshType::Knight;
    if (dynamic_cast<const Rook*>(&piece) != nullptr) return Renderer3D::PieceMeshType::Rook;
    return Renderer3D::PieceMeshType::Pawn;
}

glm::vec3 scaleForPiece(const Piece& piece) {
    constexpr float globalScale = 2.0f; // scalaire pour taille globale
    if (dynamic_cast<const Pawn*>(&piece) != nullptr) return glm::vec3(0.75f, 0.75f, 0.75f) * globalScale;
    if (dynamic_cast<const Rook*>(&piece) != nullptr) return glm::vec3(0.82f) * globalScale;
    if (dynamic_cast<const Knight*>(&piece) != nullptr) return glm::vec3(0.82f) * globalScale;
    if (dynamic_cast<const Bishop*>(&piece) != nullptr) return glm::vec3(0.82f) * globalScale;
    if (dynamic_cast<const Queen*>(&piece) != nullptr) return glm::vec3(0.88f) * globalScale;
    return glm::vec3(0.88f) * globalScale;
}

struct PieceOrientation {
    float pitch = 0.f; 
    float yaw   = 0.f; 
    float roll  = 0.f; 
};

PieceOrientation orientationForPiece(const Piece& piece) {
    return {
        0.f,                                                
        (piece.getColor() == Color::White) ? 90.f : -90.f, 
        0.f                                                 
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
    // On garde l'inversion SEULEMENT sur la profondeur (Z en 3D / Y en 2D)
    const float mappedX = static_cast<float>(pos.x); 
    const float mappedZ = static_cast<float>(kBoardCellCount - 1 - pos.y);

    const float worldX = kBoardMinCoord + 0.5f * kSquareSize + mappedX * kSquareSize;
    const float worldZ = kBoardMinCoord + 0.5f * kSquareSize + mappedZ * kSquareSize;
    
    return glm::vec3(worldX, height, worldZ);
}

glm::vec3 localOffsetForPiece(const Piece& piece) {
    return glm::vec3(0.0f, 0.0f, 0.0f);
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
    ImGuiIO& io = ImGui::GetIO();
    static ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    static const ImWchar chessRanges[] = { 0x2654, 0x265F, 0 };
    builder.AddRanges(chessRanges);
    builder.BuildRanges(&ranges);

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

    // ... ton code d'initialisation existant ...

    // Calcul de la position aléatoire à côté de l'échiquier
    // On veut X entre -5.0 et -2.0 (à gauche du plateau)
    // On veut Z entre -2.0 et 9.0 (le long du plateau)
    if (chessGame.isRandomMode()) {
        float randomX = chessGame.getRandomXDecoration();
        float randomZ = chessGame.getRandomZDecoration();
        
        m_decorationPos = glm::vec3(randomX, 0.0f, randomZ);
        
        // Bonus : une rotation aléatoire pour que ce soit plus naturel
        m_decorationRotation = chessGame.getRandomRotationDecoration();
    }
}

void App::handleCameraInput() {
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            camera.rotateLeft(io.MouseDelta.x * 0.5f);
            camera.rotateUp(io.MouseDelta.y * 0.5f);
        }
        if (io.MouseWheel != 0.0f) {
            camera.moveFront(io.MouseWheel * 1.0f);
        }
    }
    if (ImGui::IsKeyPressed(ImGuiKey_C)) {
        if (camera.getMode() == TRACKBALL) {
            camera.setMode(PIECE_VIEW);
        } else {
            camera.setMode(TRACKBALL);
        }
    }
}

void App::drawBoard(const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix, const Position& hoveredSquare) {
    if (!program || program->getGLId() <= 0 || !renderer) return;

    constexpr float kSquareHeight = 0.30f;
    constexpr float kBorderHeight = 0.40f;

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
    
    // --- GESTION DU TIMING DES DEUX PHASES D'ANIMATION ---
    float globalT = std::clamp(pathTime / pathDuration, 0.0f, 1.0f);
    float boardT = std::clamp(globalT / 0.5f, 0.0f, 1.0f);
    float pieceT = std::clamp((globalT - 0.5f) / 0.5f, 0.0f, 1.0f);

    const Position pathStart = chessGame.getPathStart();
    const Position pathTarget = chessGame.getPathTarget();
    
    Piece* selectedPiece = chessGame.getSelectedPiece();
    const auto& validMoves = chessGame.getHighlights();

    for (int boardX = 0; boardX < kBoardCellCount; ++boardX) {
        for (int boardZ = 0; boardZ < kBoardCellCount; ++boardZ) {
            
            // --- 1. ETATS DE LA CASE ---
            const bool isLightSquare = ((boardX + boardZ) % 2 == 0);
            bool isHovered = (boardX == hoveredSquare.x && boardZ == hoveredSquare.y);
            bool isSelected = (selectedPiece != nullptr && selectedPiece->getPos().x == boardX && selectedPiece->getPos().y == boardZ);
            
            bool isPossibleMove = false; 
            for (const auto& pos : validMoves) {
                if (pos.x == boardX && pos.y == boardZ) {
                    isPossibleMove = true;
                    break;
                }
            }

            // --- 2. COULEUR DE BASE ---
            glm::vec4 squareColor = isLightSquare
                ? glm::vec4(238.0f / 255.0f, 238.0f / 255.0f, 210.0f / 255.0f, 1.0f)
                : glm::vec4(118.0f / 255.0f, 150.0f / 255.0f, 86.0f / 255.0f, 1.0f);

            float elevation = 0.0f;

            // --- 3. MODIFICATEURS VISUELS SELON L'ÉTAT ---
            if (isSelected) {
                squareColor = glm::vec4(1.0f, 0.9f, 0.3f, 1.0f); 
            } else if (isPossibleMove) {
                Piece* targetPiece = board.getPiece({boardX, boardZ});
                if (targetPiece != nullptr && targetPiece->getColor() != chessGame.getTurn()) {
                    squareColor = glm::mix(squareColor, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f), 0.6f); 
                } else {
                    squareColor = glm::mix(squareColor, glm::vec4(0.2f, 0.8f, 1.0f, 1.0f), 0.5f); 
                }
                elevation += 0.05f; 
            } else if (isHovered) {
                squareColor = glm::mix(squareColor, glm::vec4(1.0f), 0.3f); 
            }

            if (pathAnimating) {
                for (size_t i = 0; i < pathSquares.size(); ++i) {
                    if (pathSquares[i].x == boardX && pathSquares[i].y == boardZ) {
                        const float phase = std::clamp(
                            boardT * (static_cast<float>(pathSquares.size()) + 1.0f) - static_cast<float>(i),
                            0.0f,
                            1.0f
                        );
                        elevation = std::sin(phase * glm::pi<float>()) * 0.25f;
                        squareColor = glm::mix(squareColor, glm::vec4(0.2f, 0.8f, 1.0f, 1.0f), 0.4f);
                        break;
                    }
                }
            }

            // --- 4. DESSIN DE LA CASE ---
            glUniform4fv(uColorLocation, 1, glm::value_ptr(squareColor));
            const glm::vec3 squareCenter = boardCenterForPosition({boardX, boardZ}, elevation + kSquareHeight * 0.5f);
            glm::mat4 squareModel = glm::translate(glm::mat4(1.0f), squareCenter);
            squareModel = glm::scale(squareModel, glm::vec3(kSquareSize, kSquareHeight, kSquareSize));
            glm::mat4 squareMvp = ProjMatrix * ViewMatrix * squareModel;
            glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(squareMvp));
            renderer->drawCube();

            // --- 5. GESTION ET DESSIN DE LA PIÈCE ---
            Piece* piece = board.getPiece({boardX, boardZ});
            if (piece == nullptr) continue;

            glm::vec4 pieceColor = (piece->getColor() == Color::White)
                ? glm::vec4(0.92f, 0.92f, 0.92f, 1.0f)
                : glm::vec4(0.18f, 0.18f, 0.18f, 1.0f);
            
            float verticalOffset = 0.0f;

            if (isSelected) {
                pieceColor = glm::mix(pieceColor, glm::vec4(1.0f, 0.9f, 0.3f, 1.0f), 0.4f);
                verticalOffset = 0.2f; 
            } else if (isHovered) {
                pieceColor = glm::mix(pieceColor, glm::vec4(1.0f), 0.2f);
            }

            glUniform4fv(uColorLocation, 1, glm::value_ptr(pieceColor));

            const auto pieceType = meshTypeForPiece(*piece);
            const auto pieceScale = scaleForPiece(*piece);
            const Position boardPos{boardX, boardZ};
            
            glm::vec3 squareCenterForPiece;
            bool isTheAnimatingPiece = false;
            if (pathAnimating) {
                if (boardX == pathStart.x && boardZ == pathStart.y) {
                    isTheAnimatingPiece = true;
                } else if (boardX == pathTarget.x && boardZ == pathTarget.y && board.getPiece(pathStart) == nullptr) {
                    isTheAnimatingPiece = true;
                }
            }

            

            if (isTheAnimatingPiece) {
                glm::vec3 startPos3D = boardCenterForPosition(pathStart, kSquareHeight);
                glm::vec3 targetPos3D = boardCenterForPosition(pathTarget, kSquareHeight);
                squareCenterForPiece = glm::mix(startPos3D, targetPos3D, pieceT);
                float jumpHeight = chessGame.getJumpHeight();
                squareCenterForPiece.y += std::sin(pieceT * glm::pi<float>()) * jumpHeight;
            } else {
                squareCenterForPiece = boardCenterForPosition(boardPos, elevation + kSquareHeight + verticalOffset);
            }

            glm::mat4 pieceModel = glm::translate(glm::mat4(1.0f), squareCenterForPiece);
            pieceModel = pieceModel * facingRotationForPiece(*piece);
            pieceModel = glm::translate(pieceModel, localOffsetForPiece(*piece));
            pieceModel = glm::scale(pieceModel, pieceScale);

            glm::mat4 pieceMvp = ProjMatrix * ViewMatrix * pieceModel;
            glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(pieceMvp));
            renderer->drawPiece(pieceType);
        }
    }

    // --- 6. DESSIN DES BORDURES DU PLATEAU ---
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
    const float innerSpan = kBoardMaxCoord - kBoardMinCoord; 
    const float fullSpan = innerSpan + (kBorderThickness * 2.0f); 

    drawBorder(glm::vec3(kBoardCenterCoord, halfBorderHeight, outerMin), glm::vec3(fullSpan, kBorderHeight, kBorderThickness));
    drawBorder(glm::vec3(kBoardCenterCoord, halfBorderHeight, outerMax), glm::vec3(fullSpan, kBorderHeight, kBorderThickness));
    drawBorder(glm::vec3(outerMin, halfBorderHeight, kBoardCenterCoord), glm::vec3(kBorderThickness, kBorderHeight, innerSpan));
    drawBorder(glm::vec3(outerMax, halfBorderHeight, kBoardCenterCoord), glm::vec3(kBorderThickness, kBorderHeight, innerSpan));

    // --- DESSIN DE L'OBJET DÉCORATIF ---
    // On lui donne une couleur spécifique (ex: gris pierre)
    if (chessGame.isRandomMode()) {
        glm::vec4 decoColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glUniform4fv(uColorLocation, 1, glm::value_ptr(decoColor));

        glm::mat4 decoModel = glm::translate(glm::mat4(1.0f), m_decorationPos);
        decoModel = glm::rotate(decoModel, glm::radians(m_decorationRotation), glm::vec3(0, 1, 0));
        decoModel = glm::scale(decoModel, glm::vec3(5.5f)); // Ajuste la taille selon ton .obj

        glm::mat4 decoMvp = ProjMatrix * ViewMatrix * decoModel;
        glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(decoMvp));
        
        renderer->drawPiece(Renderer3D::PieceMeshType::Decoration);
    }
}

void App::render() {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - lastTime).count();
    totalTime += dt;
    lastTime = now;

    // =========================================================
    // 1. GESTION DU MENU PRINCIPAL
    // =========================================================
    if (chessGame.getState() == GameState::MainMenu) {
        // On nettoie l'écran avec une couleur de fond différente pour le menu
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // On dessine l'interface du menu (la vue ImGui)
        if (chessFont) ImGui::PushFont(chessFont);
        view.draw(chessGame); 
        if (chessFont) ImGui::PopFont();
        
        // IMPORTANT : On s'arrête ici. Pas de plateau 3D ni de fenêtres de jeu !
        return; 
    }

    // =========================================================
    // 2. LOGIQUE EN JEU (Si on n'est pas dans le menu)
    // =========================================================
    chessGame.updatePathAnimation(dt);
    handleCameraInput(); 

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
            camera.setPiecePosition(basePos + glm::vec3(0.0f, 0.9f, 0.0f));
        }
    }

    Color currentTurn = chessGame.getTurn();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float screenWidth = static_cast<float>(viewport[2]);
    float screenHeight = static_cast<float>(viewport[3]);
    if (screenHeight == 0.0f) screenHeight = 1.0f; 
    float aspectRatio = screenWidth / screenHeight;

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(45.f), aspectRatio, 0.1f, 100.f);
    glm::mat4 ViewMatrix = camera.getViewMatrix(); 

    // --- RAYCASTING ET GESTION DES CLICS ---
    Position hoveredSquare = {-1, -1};
    ImGuiIO& io = ImGui::GetIO();
    
    // Seulement si on est en train de jouer
    if (chessGame.getState() == GameState::Playing && !io.WantCaptureMouse) {
        if (getHoveredSquare(io.MousePos.x, io.MousePos.y, screenWidth, screenHeight, ViewMatrix, ProjMatrix, hoveredSquare)) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                chessGame.handleSquareClick(hoveredSquare);
            }
        }
    }

    // Affichage de la 3D
    drawBoard(ViewMatrix, ProjMatrix, hoveredSquare);

    // --- UI ImGui EN JEU ---
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("Chess Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Mode : %s", chessGame.isRandomMode() ?  "Chaos" : "Normale");
    ImGui::Text("Tour : %s", currentTurn == Color::White ? "Blancs" : "Noirs");
    
    // if (chessGame.getState() == GameState::WhiteWins) ImGui::TextColored(ImVec4(0,1,0,1), "Les Blancs Gagnent !");
    // if (chessGame.getState() == GameState::BlackWins) ImGui::TextColored(ImVec4(0,1,0,1), "Les Noirs Gagnent !");
    if (chessGame.getState() == GameState::Promotion) ImGui::TextColored(ImVec4(1,1,0,1), "Promotion en cours...");

    if (chessFont) ImGui::PushFont(chessFont);
    view.draw(chessGame); // Affiche la grille 2D et les popups de fin/promotion
    if (chessFont) ImGui::PopFont();
    ImGui::End();
    
    ImGui::SetNextWindowPos(ImVec2(420, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("3D View Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Clic molette + glisser pour tourner");
    ImGui::Text("Molette pour zoomer");
    if (ImGui::Button("Changer Mode Camera")) {
        if (camera.getMode() == TRACKBALL) camera.setMode(PIECE_VIEW);
        else camera.setMode(TRACKBALL);
    }
    ImGui::Text("Mode actuel: %s", camera.getMode() == TRACKBALL ? "Trackball" : "Piece");
    ImGui::End();
}