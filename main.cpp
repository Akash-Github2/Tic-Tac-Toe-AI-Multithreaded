#include <bits/stdc++.h>
using namespace std;
#define vt vector

void initBoard(vt<vt<char>> board) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = '-';
        }
    }
}

void printBoard(vt<vt<char>> board) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cout << board[i][j];
        }
        cout << endl;
    }
}

bool playerWon(vt<vt<char>> board, bool isX) {
    char player = isX ? 'X' : 'O';
    //check rows/columns
    for (int i = 0; i < 3; ++i) {
        bool isWinR = true;
        bool isWinC = true;
        for (int j = 0; j < 3; ++j) {
            isWinR &= (board[i][j] == player);
            isWinC &= (board[j][i] == player);
        }
        if (isWinR || isWinC)
            return true;
    }

    //check diagonals
    bool isWinLR = true;
    bool isWinRL = true;
    for (int i = 0; i < 3; ++i) {
        isWinLR &= (board[i][i] == player);
        isWinRL &= (board[2-i][i] == player);
    }
    return isWinLR || isWinRL;
}

void doUserMove(vt<vt<char>> &board, char user) {

    int i, j;
    while (true) {
        cout << "Select the i,j position you want to move to: \n";
        string move;
        cin >> move;
        i = move.at(0) - '0';
        j = move.at(2) - '0';
        if (i >= 0 && i < 3 && j >= 0 && j < 3 && board[i][j] == '-') {
            break;
        }
        cout << "Move is not formatted correctly.  Should be row,col.  Try again. \n";
    }

    board[i][j] = user;
}

int findBestMove(vt<vt<char>> &board, bool isComp, int empty, char currPlayer, int depth) {
    
    int optVal = isComp ? INT_MIN : INT_MAX;
    vt<int> optPositions; //array of ints within 0-8 (i*3+j)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == '-') {

                board[i][j] = currPlayer;
                int score;

                if (playerWon(board, currPlayer == 'X') && isComp) {
                    score = 1 + (9 - depth); 
                } else if (playerWon(board, currPlayer == 'X') && !isComp) {
                    score = -1 - (9 - depth);
                } else if (empty == depth + 1) { //tie
                    score = 0;
                } else {
                    score = findBestMove(board, !isComp, empty, currPlayer == 'X' ? 'O' : 'X', depth + 1);
                }

                if (isComp) {
                    if (score > optVal) {
                        optPositions.clear();
                        optPositions.push_back(i*3+j);
                        optVal = score;
                    } else if (score == optVal) {
                        optPositions.push_back(i*3+j);
                    }
                } else {
                    if (score < optVal) {
                        optPositions.clear();
                        optPositions.push_back(i*3+j);
                        optVal = score;
                    } else if (score == optVal) {
                        optPositions.push_back(i*3+j);
                    }
                }
                board[i][j] = '-';
            }
        }
    }

    if (depth == 0) { //finally figured out best move
        int ind = rand() % optPositions.size();
        int pos = optPositions[ind];
        int i = pos / 3;
        int j = pos % 3;
        board[i][j] = currPlayer;
    }

    return optVal;
}

int main() {

    vt<vt<char>> board(3, vt<char>(3, '-'));
    initBoard(board);
    
    cout << "Do you want to be X or O (X/O)\n";
    string xo;
    cin >> xo;
    char comp = 'X';
    char user = 'O';
    bool isCompTurn = true;

    if (xo == "X") {
        user = 'X';
        comp = 'O';
        isCompTurn = false;
    }

    cout << "Initial Game Board:\n";
    printBoard(board);

    for (int i = 0; i < 9; ++i) { //num empty squares = 9 - i
        if (isCompTurn) {
            //picks random position for now - change to AI later
            // for (int i = 0; i < 3; ++i) {
            //     bool didMove = false;
            //     for (int j = 0; j < 3; ++j) {
            //         if (board[i][j] == '-') {
            //             board[i][j] = comp;
            //             didMove = true;
            //             break;
            //         }
            //     }
            //     if (didMove)
            //         break;
            // }
            findBestMove(board, true, 9 - i, comp, 0);

            cout << "Current Game Board:\n";
            printBoard(board);

            if (playerWon(board, comp == 'X')) {
                cout << "Computer wins!\n";
                break;
            } else if (i == 8) { //tie
                cout << "It's a tie\n";
                break;
            }

            isCompTurn = false; //switch player
        } else {
            doUserMove(board, user);
            cout << "Current Game Board:\n";
            printBoard(board);

            if (playerWon(board, user == 'X')) {
                cout << "You win!!!\n";
                break;
            } else if (i == 8) { //tie
                cout << "It's a tie\n";
                break;
            }

            isCompTurn = true; //switch player
        }
    }

}