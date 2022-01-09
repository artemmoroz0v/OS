#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include "MappedFile.h"
#include "CommonMutex.h"
#include "PlayerAndGame.h"
#include <fstream>

int main() {
    Player creator;
    Player connector;
    Game game;
    MappedFile mapped_file;
    std:: string client_message = "";
    mapped_file.fd = shm_open(_BUFFER_NAME, O_RDWR | O_CREAT, _SHM_OPEN_MODE);
    if (mapped_file.fd == -1) {
        std:: cout << "Error with shm_open function has been detected!" << std:: endl;
        exit(EXIT_FAILURE);
    }
    if (ftruncate(mapped_file.fd, _MAPPED_SIZE) == -1) {
        std:: cout << "An error while ftruncate has been detected!" << std:: endl;
        exit(EXIT_FAILURE);
    }
    mapped_file.data = (char *)mmap(0, _MAPPED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mapped_file.fd, 0);
    if (mapped_file.data == MAP_FAILED) {
        std:: cout << "An error with mmap function has been detected!" << std:: endl;
        exit(EXIT_FAILURE);
    }
    memset(mapped_file.data, '\0', _MAPPED_SIZE);
    CommonMutex mutex = shared_mutex_init(_MUTEX_NAME);
    if (mutex.created == 0) {
        std:: cout << "FROM SERVER: Mutex has been already created!" << std:: endl;
    } 
    else {
        errno = 0;
    }
    std:: cout << "Server is working now! Please start a game and it will be displayed here!" << std:: endl;
    while (true) {
        if (mapped_file.data[0] == EOF) {
            break;
        }
        if (mapped_file.data[0] == '\0') {
            continue;
        }
        if (!(mapped_file.data[0] == 'O' && mapped_file.data[1] == 'N' &&
              mapped_file.data[2] == _MSG_SEP)) {
            continue;
        }
        std:: cout << "FROM SERVER: Locking mutex" << std:: endl;
        if (pthread_mutex_lock(mutex.ptr) != 0) {
            std:: cout << "An error while locking mutex has been detected!" << std:: endl;
            exit(EXIT_FAILURE);
        }
        client_message = mapped_file.data;
        std:: cout << "FROM SERVER: Has received next message from client: " << client_message << std:: endl;
        memset(mapped_file.data, '\0', _MAPPED_SIZE);
        std:: vector<std:: string> client_commands;
        std:: string string = "";
        for (int i = 0; i < client_message.size(); ++i) {
            if (client_message[i] == _MSG_SEP) {
                client_commands.push_back(string);
                string = "";
            } 
            else {
                string.push_back(client_message[i]);
            }
        }
        if (client_commands[2] == "create") {
            if (game.created || game.name == client_commands[3]) {
                std:: string to = "TO";
                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "zeroplaces" + _MSG_SEP;
                sprintf(mapped_file.data, "%s", player_message.c_str());
                std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
            }
            else {
                game.created = true;
                creator.turn = true;
                connector.turn = false;
                creator.username = client_commands[1];
                RandomLocation(creator.field);
                game.name = client_commands[3];
                game.password = client_commands[4];
                std:: string to = "TO";
                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "gamecreated" + _MSG_SEP;
                sprintf(mapped_file.data, "%s", player_message.c_str());
                std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
            }
        }
        else if (client_commands[2] == "connect") {
            if (game.connected) {
                std:: string to = "TO";
                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "zeroplaces" + _MSG_SEP;
                sprintf(mapped_file.data, "%s", player_message.c_str());
                std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
            }
            else {
                if (game.name == client_commands[3]) {
                    if (game.password == client_commands[4]) {
                        game.connected = true;
                        connector.turn = false;
                        creator.turn = true;
                        connector.username = client_commands[1];
                        RandomLocation(connector.field);
                        std:: string to = "TO";
                        std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "connected" + _MSG_SEP;
                        sprintf(mapped_file.data, "%s", player_message.c_str());
                        std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                    }  
                    else {
                        game.connected = false;
                        std:: string to = "TO";
                        std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "wrongpassword" + _MSG_SEP;
                        sprintf(mapped_file.data, "%s", player_message.c_str());
                        std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                    }
                }
                else {
                    game.connected = false;
                    std:: string to = "TO";
                    std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "gamenotexists" + _MSG_SEP;
                    sprintf(mapped_file.data, "%s", player_message.c_str());
                    std:: cout << "FROM SERVER: Sending to client next message:" << player_message << std:: endl;
                }
            }
        }
        else if (client_commands[2] == "shoot") {
            if (!game.connected) {
                std:: string to = "TO";
                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "notatgame" + _MSG_SEP;
                sprintf(mapped_file.data, "%s", player_message.c_str());
                std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
            }
            if (client_commands[1] == connector.username) {
                if (connector.turn && !creator.turn) {
                    if (game.name == client_commands[3]) {
                        int number = std:: stoi(client_commands[5]);
                        std:: string l = client_commands[4];
                        char letter = l[0];
                        if (creator.field[number][int(letter) - int('A') + 1] == 'X' && 
                        (creator.field[number][int(letter) - int('A') + 2] == '.' || creator.field[number][int(letter) - int('A') + 2] == 'm' || creator.field[number][int(letter) - int('A') + 2] == 'w') && 
                        (creator.field[number - 1][int(letter) - int('A') + 1] == '.' || creator.field[number - 1][int(letter) - int('A') + 1] == 'm' || creator.field[number - 1][int(letter) - int('A') + 1] == 'w') && 
                        (creator.field[number - 1][int(letter) - int('A') + 2] == '.' || creator.field[number - 1][int(letter) - int('A') + 2] == 'm' || creator.field[number - 1][int(letter) - int('A') + 2] == 'w') &&
                        (creator.field[number + 1][int(letter) - int('A') + 1] == '.' || creator.field[number + 1][int(letter) - int('A') + 1] == 'm' || creator.field[number + 1][int(letter) - int('A') + 1] == 'w') &&
                        (creator.field[number + 1][int(letter) - int('A') + 2] == '.' || creator.field[number + 1][int(letter) - int('A') + 2] == 'm' || creator.field[number + 1][int(letter) - int('A') + 2] == 'w')) {
                            creator.field[number][int(letter) - int('A') + 1] = 'w';
                            connector.wounds++;
                            connector.kills++;
                            connector.turn = true;
                            creator.turn = false;
                            if (WonGame(creator.field)) {
                                std:: string to = "TO";
                                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youwon" + _MSG_SEP;
                                sprintf(mapped_file.data, "%s", player_message.c_str());
                                std:: cout << "FROM SERVER: Sending to connector next message:" << player_message << std:: endl;
                                connector.wins++;
                                creator.loses++;
                                std:: ofstream fout("Statistics.txt", std::ios_base::app);
                                fout << connector.username << ": " << connector.wins << " wins, " << connector.loses << " loses, " << connector.kills << " kills, " << connector.misses << " misses, " << connector.wounds << " wounds, " << std:: endl;
                                fout << creator.username << ": " << creator.wins << " wins, " << creator.loses << " loses, " << creator.kills << " kills, " << creator.misses << " misses, " << creator.wounds << " wounds, " << std:: endl;
                                creator.ErasePlayer();
                                connector.ErasePlayer();
                                PrepareField(creator.field);
                                PrepareField(connector.field);
                                game.EraseGame();
                            }
                            else {
                                std:: string to = "TO";
                                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youkilled" + _MSG_SEP;
                                sprintf(mapped_file.data, "%s", player_message.c_str());
                                std:: cout << "FROM SERVER: Sending to client next message:" << player_message << std:: endl;
                            }
                        }
                        else if (creator.field[number][int(letter) - int('A') + 1] == 'w' || creator.field[number][int(letter) - int('A') + 1] == 'm') {
                            connector.turn = true;
                            creator.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "yourepeated" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message:" << player_message << std:: endl;  
                        }
                        else if (creator.field[number][int(letter) - int('A') + 1] == 'X' && 
                        creator.field[number][int(letter) - int('A') + 2] == 'X' && 
                        (creator.field[number - 1][int(letter) - int('A') + 1] == '.' || creator.field[number - 1][int(letter) - int('A') + 1] == 'm' || creator.field[number - 1][int(letter) - int('A') + 1] == 'w') && 
                        (creator.field[number - 1][int(letter) - int('A') + 2] == '.' || creator.field[number - 1][int(letter) - int('A') + 2] == 'm' || creator.field[number - 1][int(letter) - int('A') + 2] == 'w') &&
                        (creator.field[number + 1][int(letter) - int('A') + 1] == '.' || creator.field[number + 1][int(letter) - int('A') + 1] == 'm' || creator.field[number + 1][int(letter) - int('A') + 1] == 'w') &&
                        (creator.field[number + 1][int(letter) - int('A') + 2] == '.' || creator.field[number + 1][int(letter) - int('A') + 2] == 'm' || creator.field[number + 1][int(letter) - int('A') + 2] == 'w')) {
                            creator.field[number][int(letter) - int('A') + 1] = 'w';
                            connector.wounds++;
                            connector.turn = true;
                            creator.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youwounded" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (creator.field[number][int(letter) - int('A') + 1] == 'X' && (creator.field[number][int(letter) - int('A') + 2] == '.' || creator.field[number][int(letter) - int('A') + 2] == 'm' || creator.field[number][int(letter) - int('A') + 2] == 'w') && 
                        creator.field[number - 1][int(letter) - int('A') + 1] == 'X' && 
                        (creator.field[number - 1][int(letter) - int('A') + 2] == '.' || creator.field[number - 1][int(letter) - int('A') + 2] == 'm' || creator.field[number - 1][int(letter) - int('A') + 2] == 'w') &&
                        (creator.field[number + 1][int(letter) - int('A') + 1] == '.' || creator.field[number + 1][int(letter) - int('A') + 1] == 'm' || creator.field[number + 1][int(letter) - int('A') + 1] == 'w') &&
                        (creator.field[number + 1][int(letter) - int('A') + 2] == '.' || creator.field[number + 1][int(letter) - int('A') + 2] == 'm' || creator.field[number + 1][int(letter) - int('A') + 2] == 'w')) {
                            creator.field[number][int(letter) - int('A') + 1] = 'w';
                            connector.wounds++;
                            connector.turn = true;
                            creator.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youwounded" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (creator.field[number][int(letter) - int('A') + 1] == 'X' && 
                        (creator.field[number][int(letter) - int('A') + 2] == '.' || creator.field[number][int(letter) - int('A') + 2] == 'm' || creator.field[number][int(letter) - int('A') + 2] == 'w') && 
                        (creator.field[number - 1][int(letter) - int('A') + 1] == '.' || creator.field[number - 1][int(letter) - int('A') + 1] == 'm' || creator.field[number - 1][int(letter) - int('A') + 1] == 'w') && 
                        (creator.field[number - 1][int(letter) - int('A') + 2] == '.' || creator.field[number - 1][int(letter) - int('A') + 2] == 'm' || creator.field[number - 1][int(letter) - int('A') + 2] == 'w') &&
                        creator.field[number + 1][int(letter) - int('A') + 1] == 'X' &&
                        (creator.field[number + 1][int(letter) - int('A') + 2] == '.' || creator.field[number + 1][int(letter) - int('A') + 2] == 'm' || creator.field[number + 1][int(letter) - int('A') + 2] == 'w')) {
                            creator.field[number][int(letter) - int('A') + 1] = 'w';    
                            connector.wounds++;
                            connector.turn = true;
                            creator.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youwounded" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (creator.field[number][int(letter) - int('A') + 1] == 'X' && creator.field[number + 1][int(letter) - int('A') + 1] == 'X') {
                            creator.field[number][int(letter) - int('A') + 1] = 'w';    
                            connector.wounds++;
                            connector.turn = true;
                            creator.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] +
                                                _MSG_SEP + "youwounded" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (creator.field[number][int(letter) - int('A') + 1] == '.') {
                            connector.misses++;
                            connector.turn = false;
                            creator.turn = true;
                            creator.field[number][int(letter) - int('A') + 1] = 'm';
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youmissed" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        std:: cout << "Current state of " << creator.username << "'s field is: " << std:: endl;
                        PrintField(creator.field);
                    }
                    else {
                        std:: string to = "TO";
                        std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "gamenotexists" + _MSG_SEP;
                        sprintf(mapped_file.data, "%s", player_message.c_str());
                        std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                    }
                }
                else {
                    std:: string to = "TO";
                    std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "notyourturn" + _MSG_SEP;
                    sprintf(mapped_file.data, "%s", player_message.c_str());
                    std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                }
            }
            else if (client_commands[1] == creator.username) {
                if (creator.turn && !connector.turn) {
                    if (game.name == client_commands[3]) {
                        int number = std::stoi(client_commands[5]);
                        std:: string l = client_commands[4];
                        char letter = l[0];
                        if (connector.field[number][int(letter) - int('A') + 1] == 'X' && 
                        (connector.field[number][int(letter) - int('A') + 2] == '.' || connector.field[number][int(letter) - int('A') + 2] == 'm' || connector.field[number][int(letter) - int('A') + 2] == 'w') && 
                        (connector.field[number - 1][int(letter) - int('A') + 1] == '.' || connector.field[number - 1][int(letter) - int('A') + 1] == 'm' || connector.field[number - 1][int(letter) - int('A') + 1] == 'w') && 
                        (connector.field[number - 1][int(letter) - int('A') + 2] == '.' || connector.field[number - 1][int(letter) - int('A') + 2] == 'm' || connector.field[number - 1][int(letter) - int('A') + 2] == 'w') &&
                        (connector.field[number + 1][int(letter) - int('A') + 1] == '.' || connector.field[number + 1][int(letter) - int('A') + 1] == 'm' || connector.field[number + 1][int(letter) - int('A') + 1] == 'w') &&
                        (connector.field[number + 1][int(letter) - int('A') + 2] == '.' || connector.field[number + 1][int(letter) - int('A') + 2] == 'm' || connector.field[number + 1][int(letter) - int('A') + 2] == 'w')) {
                            connector.field[number][int(letter) - int('A') + 1] = 'w';
                            creator.kills++;
                            creator.wounds++;
                            creator.turn = true;
                            connector.turn = false;
                            if (WonGame(connector.field)) {
                                std:: string to = "TO";
                                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youwon" + _MSG_SEP;
                                sprintf(mapped_file.data, "%s", player_message.c_str());
                                std:: cout << "FROM SERVER: Sending to creator next message: " << player_message << std:: endl;
                                creator.wins++;
                                connector.loses++;
                                std:: ofstream fout("Statistics.txt", std::ios_base::app);
                                fout << connector.username << ": " << connector.wins << " wins, " << connector.loses << " loses, " << connector.kills << " kills, " << connector.misses << " misses, " << connector.wounds << " wounds." << std:: endl;
                                fout << creator.username << ": " << creator.wins << " wins, " << creator.loses << " loses, " << creator.kills << " kills, " << creator.misses << " misses, " << creator.wounds << " wounds. " << std:: endl;
                                creator.ErasePlayer();
                                connector.ErasePlayer();
                                PrepareField(creator.field);
                                PrepareField(connector.field);
                                game.EraseGame();
                            }
                            else {
                                std:: string to = "TO";
                                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youkilled" + _MSG_SEP;
                                sprintf(mapped_file.data, "%s", player_message.c_str());
                                std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                            }
                        }
                        else if (connector.field[number][int(letter) - int('A') + 1] == 'w' || connector.field[number][int(letter) - int('A') + 1] == 'm') {
                            creator.turn = true;
                            connector.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "yourepeated" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (connector.field[number][int(letter) - int('A') + 1] == 'X' && 
                        connector.field[number][int(letter) - int('A') + 2] == 'X' && 
                        (connector.field[number - 1][int(letter) - int('A') + 1] == '.' || connector.field[number - 1][int(letter) - int('A') + 1] == 'm' || connector.field[number - 1][int(letter) - int('A') + 1] == 'w') && 
                        (connector.field[number - 1][int(letter) - int('A') + 2] == '.' || connector.field[number - 1][int(letter) - int('A') + 2] == 'm' || connector.field[number - 1][int(letter) - int('A') + 2] == 'w') &&
                        (connector.field[number + 1][int(letter) - int('A') + 1] == '.' || connector.field[number + 1][int(letter) - int('A') + 1] == 'm' || connector.field[number + 1][int(letter) - int('A') + 1] == 'w') &&
                        (connector.field[number + 1][int(letter) - int('A') + 2] == '.' || connector.field[number + 1][int(letter) - int('A') + 2] == 'm' || connector.field[number + 1][int(letter) - int('A') + 2] == 'w')) {
                            connector.field[number][int(letter) - int('A') + 1] = 'w';
                            creator.wounds++;
                            creator.turn = true;
                            connector.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youwounded" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (connector.field[number][int(letter) - int('A') + 1] == 'X' && (connector.field[number][int(letter) - int('A') + 2] == '.' || connector.field[number][int(letter) - int('A') + 2] == 'm' || connector.field[number][int(letter) - int('A') + 2] == 'w') && 
                        connector.field[number - 1][int(letter) - int('A') + 1] == 'X' && 
                        (connector.field[number - 1][int(letter) - int('A') + 2] == '.' || connector.field[number - 1][int(letter) - int('A') + 2] == 'm' || connector.field[number - 1][int(letter) - int('A') + 2] == 'w') &&
                        (connector.field[number + 1][int(letter) - int('A') + 1] == '.' || connector.field[number + 1][int(letter) - int('A') + 1] == 'm' || connector.field[number + 1][int(letter) - int('A') + 1] == 'w') &&
                        (connector.field[number + 1][int(letter) - int('A') + 2] == '.' || connector.field[number + 1][int(letter) - int('A') + 2] == 'm' || connector.field[number + 1][int(letter) - int('A') + 2] == 'w')) {
                            connector.field[number][int(letter) - int('A') + 1] = 'w';
                            creator.wounds++;
                            creator.turn = true;
                            connector.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youwounded" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (connector.field[number][int(letter) - int('A') + 1] == 'X' && 
                        (connector.field[number][int(letter) - int('A') + 2] == '.' || connector.field[number][int(letter) - int('A') + 2] == 'm' || connector.field[number][int(letter) - int('A') + 2] == 'w') && 
                        (connector.field[number - 1][int(letter) - int('A') + 1] == '.' || connector.field[number - 1][int(letter) - int('A') + 1] == 'm' || connector.field[number - 1][int(letter) - int('A') + 1] == 'w') && 
                        (connector.field[number - 1][int(letter) - int('A') + 2] == '.' || connector.field[number - 1][int(letter) - int('A') + 2] == 'm' || connector.field[number - 1][int(letter) - int('A') + 2] == 'w') &&
                        connector.field[number + 1][int(letter) - int('A') + 1] == 'X' &&
                        (connector.field[number + 1][int(letter) - int('A') + 2] == '.' || connector.field[number + 1][int(letter) - int('A') + 2] == 'm' || connector.field[number + 1][int(letter) - int('A') + 2] == 'w')) {
                            connector.field[number][int(letter) - int('A') + 1] = 'w';
                            creator.wounds++;
                            creator.turn = true;
                            connector.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youwounded" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (connector.field[number][int(letter) - int('A') + 1] == 'X' && connector.field[number + 1][int(letter) - int('A') + 1] == 'X') {
                            connector.field[number][int(letter) - int('A') + 1] = 'w';    
                            connector.wounds++;
                            connector.turn = true;
                            creator.turn = false;
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] +
                                                _MSG_SEP + "youwounded" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        else if (connector.field[number][int(letter) - int('A') + 1] == '.') {
                            creator.misses++;
                            creator.turn = false;
                            connector.turn = true;
                            connector.field[number][int(letter) - int('A') + 1] = 'm';
                            std:: string to = "TO";
                            std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "youmissed" + _MSG_SEP;
                            sprintf(mapped_file.data, "%s", player_message.c_str());
                            std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                        }
                        std:: cout << "Current state of " << connector.username << "'s field is: " << std:: endl;
                        PrintField(connector.field);
                    }
                    else {
                        std:: string to = "TO";
                        std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "gamenotexists" + _MSG_SEP;
                        sprintf(mapped_file.data, "%s", player_message.c_str());
                        std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                    }
                }
                else {
                    creator.turn = false;
                    std:: string to = "TO";
                    std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "notyourturn" + _MSG_SEP;
                    sprintf(mapped_file.data, "%s", player_message.c_str());
                    std:: cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
                }
            }
        }
        else if (client_commands[2] == "disconnect") {
            if (client_commands[1] == creator.username) {
                creator.turn = false;
                connector.turn = true;
                game.connected = false;
                std:: string to = "TO";
                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "disconnected" + _MSG_SEP;
                sprintf(mapped_file.data, "%s", player_message.c_str());
                std::cout << "FROM SERVER: Sending to client next message: " << player_message << std::endl;
            }
            else {
                creator.turn = true;
                connector.turn = false;
                game.connected = false;
                std:: string to = "TO";
                std:: string player_message = to + _MSG_SEP + connector.username + _MSG_SEP + "disconnected" + _MSG_SEP;
                sprintf(mapped_file.data, "%s", player_message.c_str());
                std::cout << "FROM SERVER: Sending to client next message: " << player_message << std:: endl;
            }
        }
        else if (client_commands[2] == "stats") {
            if (creator.username == client_commands[1]) {
                std:: string to = "TO";
                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "stats" + _MSG_SEP + std:: to_string(creator.wins) + _MSG_SEP + std:: to_string(creator.loses) + _MSG_SEP + std:: to_string(creator.kills) + _MSG_SEP + std:: to_string(creator.misses) + _MSG_SEP + std:: to_string(creator.wounds) + _MSG_SEP;
                sprintf(mapped_file.data, "%s", player_message.c_str());
                std:: cout << "FROM SERVER: Sending to creator next message: " << player_message << std:: endl;
            }
            else {
                std:: string to = "TO";
                std:: string player_message = to + _MSG_SEP + client_commands[1] + _MSG_SEP + "stats" + _MSG_SEP + std:: to_string(connector.wins) + _MSG_SEP + std:: to_string(connector.loses) + _MSG_SEP + std:: to_string(connector.kills) + _MSG_SEP + std:: to_string(connector.misses) + _MSG_SEP + std:: to_string(connector.wounds) + _MSG_SEP;
                sprintf(mapped_file.data, "%s", player_message.c_str());
                std:: cout << "FROM SERVER: Sending to connector next message: " << player_message << std::endl;
            }
        }
        pthread_mutex_unlock(mutex.ptr);
        std:: cout << "FROM SERVER: Unlocked mutex" << std:: endl;
    }
    if (shared_mutex_destroy(mutex) == -1) {
        std:: cout << "An error while destroying mutex has been detected!" << std:: endl;
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(_BUFFER_NAME) == -1) {
        std:: cout << "An error while shm_unlink has been detected!" << std:: endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}