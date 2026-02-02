#include <imgui.h>
#include <iostream>
#include <string>
#include "board.hpp"
#include "player.hpp"
#include "quick_imgui/quick_imgui.hpp"

int main()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark(); // ou Classic / Light
    ImGuiIO& io = ImGui::GetIO();

    static const ImWchar chessRanges[] = {0x2654, 0x265F, 0};

    ImFont* chessFont = io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\seguisym.ttf",
        32.0f,
        nullptr,
        chessRanges
    );

    IM_ASSERT(chessFont);

    float value{0.f};

    quick_imgui::loop(
        "Chess",
        {
            .init = [&]() {},
            .loop =
                [&]() {
                    // ImGui::ShowDemoWindow(); // This opens a window which shows tons of examples of what you can do with ImGui. You should check it out! Also, you can use the "Item Picker" in the top menu of that demo window: then click on any widget and it will show you the corresponding code directly in your IDE!

                    ImGui::Begin("Example");

                    Player player1 = Player(true, "p1");
                    Player Player2 = Player(false, "p2");

                    Board board = Board();
                    board.initializeBoard();

                    ImGui::End();
                },
        }
    );
}