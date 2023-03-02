#include <bits/stdc++.h>
#include <chrono>
#include <unistd.h>
#include <pthread.h>
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

//only called for depth >= 1
int findBestMove(vt<vt<char>> &board, bool isComp, int empty, char currPlayer, int depth) {

    int optVal = isComp ? INT_MIN : INT_MAX;
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

                //only need specific positions for depth 0, but here we don't reach that so we only need the value itself
                if (isComp) {
                    optVal = max(optVal, score);
                } else {
                    optVal = min(optVal, score);
                }
                board[i][j] = '-';
            }
        }
    }

    return optVal;
}


struct thread_data {
   int currBoard[3][3];
   bool isComp;
   int empty;
   char currPlayer;
   int* i;
   int* j;
};

void* findBestMoveInner(void *arg) {
    struct thread_data *data;
    data = (struct thread_data *) arg;

    vt<vt<char>> currBoard(3, vt<char>(3, '-'));
    for (int a = 0; a < 3; ++a) {
        for (int b = 0; b < 3; ++b) {
            currBoard[a][b] = data->currBoard[a][b];
        }
    }
    bool isComp = data->isComp;
    int empty = data->empty;
    char currPlayer = data->currPlayer;
    int* i = data->i;
    int* j = data->j;
    
    currBoard[*i][*j] = currPlayer;

    delete i;
    delete j;

    int score;

    if (playerWon(currBoard, currPlayer == 'X') && isComp) {
        score = 10; //9 - depth when depth is 0 = 9, so 1 + 9 = 10
    } else if (playerWon(currBoard, currPlayer == 'X') && !isComp) {
        score = -10;
    } else if (empty == 1) { //tie (because we didn't update empty after adding the new move, so previously it would've been)
        score = 0;
    } else {
        score = findBestMove(currBoard, !isComp, empty, currPlayer == 'X' ? 'O' : 'X', 1);
    }

    int* result = new int(score);
    return (void*) result;
}


void overallFindBestMove(vt<vt<char>> &board, bool isComp, int empty, char currPlayer) {
    pthread_t threads[9];
    int nextThreadInd = 0;
    vt<int> posForThread;
    struct thread_data currData[9];
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == '-') {
                int* newI = new int(i);
                int* newJ = new int(j);

                for (int a = 0; a < 3; ++a) {
                    for (int b = 0; b < 3; ++b) {
                        currData[nextThreadInd].currBoard[a][b] = board[a][b];
                    }
                }
                currData[nextThreadInd].isComp = isComp;
                currData[nextThreadInd].empty = empty;
                currData[nextThreadInd].currPlayer = currPlayer;
                currData[nextThreadInd].i = newI;
                currData[nextThreadInd].j = newJ;

                posForThread.push_back(i*3+j);
                if (pthread_create(&threads[nextThreadInd], NULL, &findBestMoveInner, (void *)&currData[nextThreadInd])) {
                    perror("error creating thread"); //error creating thread
                }

                nextThreadInd++;
            }
        }
    }

    int optVal = isComp ? INT_MIN : INT_MAX;
    vt<int> optPositions; //array of ints within 0-8 (i*3+j)

    for (int i = 0; i < nextThreadInd; ++i) {
        int* res;
        if (pthread_join(threads[i], (void**) &res)) {
            perror("error joining thread"); //error joining thread
        }

        int score = *res;
        delete res;
        // cout << posForThread[i] << "'s score: " << score << endl;

        //since it's only the computer here, we only need to account for that
        if (score > optVal) {
            optPositions.clear();
            optPositions.push_back(posForThread[i]);
            optVal = score;
        } else if (score == optVal) {
            optPositions.push_back(posForThread[i]);
        }
    }

    //finally figured out best move
    int ind = rand() % optPositions.size();
    int pos = optPositions[ind];
    int i = pos / 3;
    int j = pos % 3;
    // cout << i << " " << j << endl;
    board[i][j] = currPlayer;
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
            auto start = chrono::steady_clock::now();
            overallFindBestMove(board, true, 9 - i, comp);
            auto end = chrono::steady_clock::now();

            cout << "Elapsed time in milliseconds: "
                << chrono::duration_cast<chrono::milliseconds>(end - start).count()
                << " ms" << endl;

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