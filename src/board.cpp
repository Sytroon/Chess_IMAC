#include "board.hpp"

Board::Board()
    : size(8), board(size, std::vector<bool>(size, false)) {};

void Board::initializeBoard()
{
    int id = 1;
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            if ((i + j) % 2 == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{251.f / 255.f, 194.f / 255.f, 115.f / 255.f, 1.f});
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{149.f / 255.f, 83.f / 255.f, 59.f / 255.f, 1.f});
            }
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{1.f, 0.f, 0.f, 0.5f});
            ImGui::PushID(id);
            board[i][j] = ImGui::Button("", ImVec2{50.f, 50.f});
            if (j < (size - 1))
            {
                ImGui::SameLine();
            }
            ++id;
            ImGui::PopID();
            ImGui::PopStyleColor(2);
        }
    }
}
