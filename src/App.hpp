#pragma once
#include <memory>
#include <chrono>
#include <string>
#include <vector>
#include "glimac/Program.hpp"
#include "glimac/FilePath.hpp"
#include "game.hpp"
#include "chessView.hpp"
#include "Renderer3D.hpp"
#include "TrackballCamera.hpp"
#include <imgui.h>
#include "poisson.hpp"
#include "exponentielle.hpp"
#include "binomiale.hpp"

struct Star {
    glm::vec3 position;
    float size;
    float lifetime;
};


class App {
private:
    Game chessGame;
    ChessView view;
    TrackballCamera camera;

    ImFont* chessFont = nullptr;

    std::unique_ptr<Renderer3D> renderer;
    std::unique_ptr<glimac::Program> program;

    GLint uMVPLocation = -1;
    GLint uColorLocation = -1;
    GLint uLightingModeLocation = -1;
    GLint uTimeLocation = -1;

    std::chrono::steady_clock::time_point lastTime;
    float totalTime = 0.0f;

    std::vector<Star> stars;
    float starTimer = 0.0f;

    void checkGLError(const std::string& label);
    void drawBoard(const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix);
    void drawStars(const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix);
    void drawCube(const glm::vec3& position, float size, const glm::vec4& color, const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix);
    void handleCameraInput(); // Nouvelle méthode pour la souris

public:
    App();
    void init(const glimac::FilePath& applicationPath);
    void render();
};