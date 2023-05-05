#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>

using namespace std;

string formattedString(string unformatted)
{
    string formatted = "";

    for (int i = 0; i < unformatted.size(); i++)
    {
        if (unformatted[i] == '\n')
        {
            formatted += "\\n";
        }
        else
        {
            formatted += unformatted[i];
        }
    }

    return formatted;
}

string boardToString(char **board)
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

char** duplicateBoard(char** board)
{
    char** duplicate = new char* [3];
    for (int i = 0; i < 3; i++)
    {
        duplicate[i] = new char [3];
        for (int j = 0; j < 3; j++)
        {
            duplicate[i][j] = board[i][j];
        }
    }
    return duplicate;
}

char winState(char** board)
{
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
            return c;
        }
    }
    return '0';
}

string exploreTree(char** board, int x, int y, string tab, char piece, string originalTab, string printFunction)
{
    string dontLookAtThis = "";
    int index = 3*x+y+1;

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

    string newTab = tab + originalTab;
    char winner = winState(board);

    if (winner <= '9')
    {
        dontLookAtThis += newTab + printFunction+ "(\"" + formattedString(boardToString(board)) + "Player " + (piece == 'X' ? 'O' : 'X') + ", enter the number of the spot you want to set your symbol at: \");\n" + newTab;
        dontLookAtThis += "scanf(\"%d\", &move);\n";

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                dontLookAtThis += exploreTree(duplicateBoard(board), i, j, newTab, piece == 'X' ? 'O' : 'X', originalTab, printFunction);
            }
        }
        dontLookAtThis += tab + "}\n";

        if (index == 9)
        {
            dontLookAtThis += "\n" + tab + "else\n" + tab + "{\n" + newTab + printFunction + "(\"Invalid move, you suck at this game, try again.\\n\");\n";
            dontLookAtThis += newTab + "return 0;\n" + tab + "}\n";
        }

        return dontLookAtThis;
    }

    dontLookAtThis += newTab + printFunction + "(\"" + winner + " wins!\\n\");\n" + newTab + "return 0;\n" + tab + "}\n";
    return dontLookAtThis;
}

string beginExploring(int tabLength)
{
    string tab = "";
    for (int i = 0; i < tabLength; i++)
    {
        tab += ' ';
    }

    char** board = new char* [3];
    for (int i = 0; i < 3; i++)
    {
        board[i] = new char [3];
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
            terribleCode += exploreTree(duplicateBoard(board), i, j, tab, 'X', tab, "printf");
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
