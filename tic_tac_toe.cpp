/*

Modified Low-Level Design Problem: Tic Tac Toe Game with k Players on an n×n Board
Problem Statement:
You are tasked with designing a Tic Tac Toe game that supports:

An n×n board, where n can be any integer greater than or equal to 3.
k players, where k can be any integer greater than or equal to 2 (e.g., in a standard game, k = 2).

Support Undo and redo

Store player win lose statistics as well.


4:18
Entities
GameState: Stores all the move made till now
GameLoop: Class for running the game.
PlayersStats:  wins, losses, rating x
Players: name, id, stats. x
Moves x
MoveHistory x
*/
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>

using namespace std;

struct Move {
    int x;
    int y;
    int player;
    Move(int x, int y, int player): x(x), y(y), player(player) {}
};

struct MoveHistory {
    vector<Move> history;
};

struct PlayerStatistics {
    int wins, losses, rating;
    PlayerStatistics():wins(0), losses(0), rating(1500) {}
    void addWin() {
        wins += 1;
        rating += 1;
    }

    void addLoss() {
        losses += 1;
        rating -=1;
    }
};
struct Player {
    string name;
    int id;
    PlayerStatistics statistics;
    Player(const string &name, int id): name(name), id(id) {}
};


class PlayerManager {
    unordered_map<int, unique_ptr<Player>> players;
    public:
    int addPlayer(const string& name) {
        int id = rand();
        auto player = make_unique<Player>(name, id);
        players[id] = std::move(player);

        return id;
    }


    bool removePlayer(const int id) {
        if (players.find(id) == players.end()) return false;
        players.erase(id);
        return true;
    }

    void setPlayerVictory(int id) {
        if (players.find(id) == players.end()) return;
        auto &player = players.find(id)->second;
        player->statistics.addWin();
    }

    void setPlayerLoss(int id) {
        if (players.find(id) == players.end()) return;
        auto &player = players.find(id)->second;
        player->statistics.addLoss();
    }
};

class GameState {
    using PlayerId = int;
    using Updater = function<void(int, int, int)>;
    int n;
    vector<Move> history;
    int moveIndex;
    Updater updater; 
    bool winner;
    vector<int> players;
    int current_player;
    public:
    GameState(int boardSize, const vector<int> &players):n(boardSize),moveIndex(0),winner(-1),players(players),current_player(0) {
        for(auto &id : players) {
            rowMoves[id] = {};
            colMoves[id] = {};
            diagonal[id] = 0;
            antiDiagonal[id] = 0;
        }
    }

    void setUpdater(Updater updater) {
        updater = updater;
    }
    unordered_map<PlayerId,  unordered_map<int,int>> rowMoves;
    unordered_map<PlayerId, unordered_map<int,int>> colMoves;
    unordered_map<PlayerId, int> diagonal;
    unordered_map<PlayerId, int> antiDiagonal;

    void move(int x, int y, int player) {
        Move move(x, y, player);
        history.push_back(move);
        moveIndex++;
        doMove();
    }

    void doMove() {
        auto [x, y, player] = history[moveIndex];
        rowMoves[player][x]++;
        colMoves[player][y]++;
        if (x == y) diagonal[player]++;
        if (y == (n - x - 1)) antiDiagonal[player]++;
        updater(x, y, player);
    }

    void undoMove() {
        auto [x, y, player] = history[moveIndex];
        rowMoves[player][x]--;
        colMoves[player][y]--;
        if (x == y) diagonal[player]--;
        if (y == (n - x - 1)) antiDiagonal[player]--;  
        updater(x, y, -1);    
    }



    void redo() {
        if (moveIndex + 1 < history.size()) {
            moveIndex++;
            doMove();
        }
    }

    void undo() {
        if (moveIndex - 1 >= 0) {
            moveIndex--;
            undoMove();
        }
    }

    bool checkWin(int x, int y, int player) {
        if (rowMoves[player][x] == n or colMoves[player][y] == n) return true;
        if (x == y and diagonal[player] == n) return true;  
        if (y == (n - x - 1) and antiDiagonal[player] == n) return true;
        return false;
    }

    bool run() {
        int x, y;
        cin >> x >> y;
        move(x, y, players[current_player]);
        bool hasWon = checkWin(x, y, players[current_player]);
        current_player = (current_player + 1) % static_cast<int>(players.size());
        if (hasWon) return true;
        return false;
    }

    int get_current_player() {
        return players[current_player];
    }
};

class GameBoard {
    private:
    int n;
    vector<vector<char>> board;
    unordered_map<int, char> playerMapping;
    vector<int> players;
    GameState state;

    public:
    GameBoard(int n, vector<int> players): n(n), state(n, players) {
        board.resize(n, vector<char>(n, 'e'));
        char c = 'a';
        for(auto &p: players) playerMapping[p] = c++;
        state.setUpdater([&](int x, int y, int player) -> void {
            update_board(x, y, player);
        });
    }

    void update_board(int x, int y, int player) {
        if (playerMapping.find(player) == playerMapping.end()) {
            board[x][y] = 'e';
            return;
        }

        board[x][y] = playerMapping[player];
        printBoard();
    };

    void printBoard() {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) cout << board[i][j] <<" ";
            cout<<endl;
        }
    }


    void runLoop() {
        int turns = 0;
        while(turns < n * n) {
            if (state.run()) break;

            cout << "Running for another iteration " <<turns<< endl;      
            turns++; 
        }
        cout << state.get_current_player() << " wins " << endl;

    }
};

class Orchestrator {
    unique_ptr<PlayerManager> manager;
    vector<int> playerIds;
    public:
    Orchestrator() {}

    void addPlayer(const string&name) {
        playerIds.push_back(manager->addPlayer(name));
    }

    void startGame(int n) {
        GameBoard b(n, playerIds);
        b.runLoop();
    }
};

int main() {

    Orchestrator orch;
    orch.addPlayer("Ritwiz");
    orch.addPlayer("harsh");

    orch.startGame(3);
}
