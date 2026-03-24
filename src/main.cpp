#include "quick_imgui/quick_imgui.hpp"
#include "App.hpp"

// Si tu utilisais chessFont ailleurs, on le garde ici
// ImFont* chessFont = nullptr;

int main(int argc, char** argv)
{
    glimac::FilePath applicationPath(argv[0]);
    App app;

    quick_imgui::loop("Chess Project 3D", {
        .init = [&]() {
            app.init(applicationPath);
        },
        .loop = [&]() {
            app.render();
        }
    });

    return 0;
}   