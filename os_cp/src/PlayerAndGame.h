#ifndef PLAYERANDGAME_H
#define PLAYERANDGAME_H
#include <algorithm>
#include <vector>
class Player {
    public:
        std:: string username;
        std:: vector<std::vector<char>> field;
        int wins;
        int loses;
        int kills;
        int misses;
        int wounds;
        bool turn;
        Player() : wins(0), loses(0), kills(0), misses(0), wounds(0), field(12, std::vector<char> (12, '.')), username(""), turn(false) {}
        void ErasePlayer() {
            username = "";
            wins = 0;
            loses = 0;
            kills = 0;
            misses = 0;
            wounds = 0;
            turn = false;
        } 
};
class Game {
    public:
        std:: string name;
        std:: string password;
        bool connected;
        bool created;
        Game() : name(""), password(""), connected(false), created(false) {}
        void EraseGame() {
            name = "";
            password = "";
            connected = false;
            created = false;
        }
};
void RandomLocation (std::vector<std::vector<char>> &field) {
    int j =- 1, k, v, l, x[2], y;
    srand(time(0));
    for (l = 4; l > 0; l--) {
        for (k = 5; k - l; k--) {
            v = 1&rand();
            do for (x[v] = 1 + rand() % 10, x[1 - v] = 1 + rand() % 7, y = j = 0; j - l; y |= field[x[0]][x[1]] != '.', x[1 - v]++, j++); while(y);
            x[1 - v] -= l + 1, field[x[0]][x[1]] = '/', x[v]--, field[x[0]][x[1]] = '/', x [v] += 2, field[x[0]][x[1]] = '/', x[v]--, x[1 - v]++;
            for (j = -1; ++j - l; field[x[0]][x[1]] = 'X', x[v]--, field[x[0]][x[1]] = '/', x[v] += 2, field[x[0]][x[1]] = '/', x[v]--, x[1 - v]++);
            field[x[0]][x[1]] = '/', x[v]--, field[x[0]][x[1]] = '/', x[v]+=2, field[x[0]][x[1]] = '/';
        }
    }
    for (int i = 0; i < 12; ++i) {
        std::replace(field[i].begin(), field[i].end(), '/', '.');
    }
}
void PrintField (std::vector<std::vector<char>> &field) {
    for (int i = 1; i < 11; ++i) {
        for (int j = 1; j < 11; ++j) {
            std:: cout << field[i][j];
        }
        std:: cout << std:: endl;
    }
}
bool WonGame (std::vector<std::vector<char>> &field) {
    for (int i = 1; i < 11; ++i) {
        for (int j = 1; j < 11; ++j) {
            if (field[i][j] == 'X') {
                return false;
            }
        }
    }
    return true;
}
void PrepareField (std::vector<std::vector<char>>& field) {
	for (int i = 0; i < 12; i++) {
		field[i].clear();
		field[i] = std::vector<char>(12, '.');
	}
}
#endif