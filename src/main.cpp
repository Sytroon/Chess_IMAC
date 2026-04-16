#include "quick_imgui/quick_imgui.hpp"
#include "App.hpp"

int main(int argc, char** argv)
{
    glimac::FilePath applicationPath(argv[0]);
    App app;

    quick_imgui::loop("Chess Ilyass & Hugo", {
        .init = [&]() {
            app.init(applicationPath);
        },
        .loop = [&]() {
            app.render();
        }
    });

    return 0;
}   