#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <array>

using namespace std;

string formattedString(string unformatted)
{
    string formatted = "";

    for (char c : unformatted)
    {
        if (c == '\n')
        {
            formatted += "\\n";
        }
        else
        {
            formatted += c;
        }
    }

    return formatted;
}

string boardToString(std::array<std::array<char, 3>, 3> board)
{
    string fancyBoard = "";
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            fancyBoard += '[';
            fancyBoard += board[i][j];
            fancyBoard += ']';
        }
        fancyBoard += "\n";
    }
    return fancyBoard;
}

bool checkDraw(std::array<std::array<char, 3>, 3> board)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] <= '9')
            {
                return false;
            }
        }
    }

    return true;
}

enum class WinState {XWin, OWin, Draw, Unfinished};

WinState winState(std::array<std::array<char, 3>, 3> board)
{
    if (checkDraw(board)) return WinState::Draw;

    char c;
    for (int i = 0; i < 2; i++)
    {
        char c = c == 'X' ? 'O' : 'X';

        if ((board[0][0] == c && board[0][1] == c && board[0][2] == c) ||
            (board[1][0] == c && board[1][1] == c && board[1][2] == c) ||
            (board[2][0] == c && board[2][1] == c && board[2][2] == c) ||
            (board[0][0] == c && board[1][0] == c && board[2][0] == c) ||
            (board[0][1] == c && board[1][1] == c && board[2][1] == c) ||
            (board[0][2] == c && board[1][2] == c && board[2][2] == c) ||
            (board[0][0] == c && board[1][1] == c && board[2][2] == c) ||
            (board[2][0] == c && board[1][1] == c && board[0][2] == c))
        {
            if (c == 'X')
            {
                return WinState::XWin;
            }
            else
            {
                return WinState::OWin;
            }
        }
    }
    return WinState::Unfinished;
}

string exploreTree(std::array<std::array<char, 3>, 3> board, int x, int y, string tab, char piece, string originalTab, string printFunction)
{
    const string newTab = tab + originalTab;
    const int index = 3*x+y+1;
    string dontLookAtThis = "";

    //cout << boardToString(board) << '\n';

    if (board[x][y] <= '9')
    {
        dontLookAtThis += tab + "if (move == " + (char)(index + '0') + ")\n" + tab + "{\n";
    }
    else
    {
        return "";
    }

    board[x][y] = piece;

    WinState winner = winState(board);

    if (winner == WinState::Unfinished)
    {
        dontLookAtThis += newTab + printFunction+ "(\"" + formattedString(boardToString(board)) + "Player " + (piece == 'X' ? 'O' : 'X') + ", enter the number of the spot you want to set your symbol at: \");\n" + newTab;
        dontLookAtThis += "scanf(\"%d\", &move);\n";

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                dontLookAtThis += exploreTree(board, i, j, newTab, piece == 'X' ? 'O' : 'X', originalTab, printFunction);
            }
        }

        dontLookAtThis += newTab + "else\n" + newTab + "{\n" + newTab + originalTab + printFunction + "(\"Invalid move, you suck at this game, try again.\\n\");\n";
        dontLookAtThis += newTab + originalTab + "return 0;\n" + newTab + "}\n";
        dontLookAtThis += tab + "}\n";

        return dontLookAtThis;
    }

    dontLookAtThis += newTab + printFunction + "(\"" + formattedString(boardToString(board)) + "\");\n";
    if (winner == WinState::Draw)
    {
        dontLookAtThis += newTab + printFunction + "(\"Draw!\\n\");\n";
    }
    else
    {
        dontLookAtThis += newTab + printFunction + "(\"" + (winner == WinState::XWin ? 'X' : 'O') + " wins!\\n\");\n";
    }
    dontLookAtThis += newTab + "return 0;\n" + tab + "}\n";

    return dontLookAtThis;
}

string beginExploring(int tabLength)
{
    string tab = "";
    for (int i = 0; i < tabLength; i++)
    {
        tab += ' ';
    }

    std::array<std::array<char, 3>, 3> board;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = (char) (3*i+j) + '1';
        }
    }

    string terribleCode;
    //code at the start of the file
    terribleCode = "#include <stdio.h>\n\nint main()\n{\n" + tab + "int move = 0;\n" + tab + "printf(\"" + formattedString(boardToString(board)) + "Player X, enter the number of the spot you want to set your symbol at: \");\n" + tab + "scanf(\"%d\", &move);\n";

    //all the if and else statements that replace the main gameplay loop
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            terribleCode += exploreTree(board, i, j, tab, 'X', tab, "printf");
        }
    }

    //code at the end of the file
    terribleCode += tab + "return 0;\n}";
    return terribleCode;
}

int main()
{
    int tabLength;
    do
    {
        cout << "Set the amount of spaces an indent should have: ";
        cin >> tabLength;
    } while (tabLength < 0 );

    string fileName = "if-tac-toe.c";
    ofstream outputFile(fileName);

    cout << "Started generating...\n";
    auto start = chrono::steady_clock::now();

    outputFile << beginExploring(tabLength);
    outputFile.close();

    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Done! Took " << (float)(elapsed.count())/1000 << " seconds.\n";

    return 0;
}
