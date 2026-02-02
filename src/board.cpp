#include "board.hpp"
#include <cstddef>

Board::Board()
    : size(8), board(size, std::vector<bool>(size, false)), visualBoard(size, std::vector<std::string>(size, ""))
{
    prepareBoard();
};

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
            board[i][j] = ImGui::Button(visualBoard[i][j].c_str(), ImVec2{50.f, 50.f});
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

void Board::prepareBoard()
{
    visualBoard[0] = {
        "\u2656", "\u2658", "\u2657", "\u2655",
        "\u2654", "\u2657", "\u2658", "\u2656"
    };

    visualBoard[1] = {
        "\u2659", "\u2659", "\u2659", "\u2659",
        "\u2659", "\u2659", "\u2659", "\u2659"
    };

    visualBoard[6] = {
        "\u265F", "\u265F", "\u265F", "\u265F",
        "\u265F", "\u265F", "\u265F", "\u265F"
    };

    visualBoard[7] = {
        "\u265C", "\u265E", "\u265D", "\u265B",
        "\u265A", "\u265D", "\u265E", "\u265C"
    };
}

Position Board::checkClick()
{
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (board[i][j])
            {
                return Position(i, j);
            }
        }
    }
}

void Board::playTurn()
{
    bool wrongPos = true;
    while (wrongPos)
    {
        // Position pos = checkClick();
        // for (Piece)
    }
}
