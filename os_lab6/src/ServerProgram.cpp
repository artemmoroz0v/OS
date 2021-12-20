#include <bits/stdc++.h>
#include "CalculationNode.h"
#include "ZMQFunctions.h"
#include "BalancedTree.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std:: cout << "Usage: 1)./main, 2) child_id, 3) parent_port, 4) parent_id" << std:: endl;
        exit(EXIT_FAILURE);
    }
    CalculationNode node(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    while(true) {
        std:: string message;
        std:: string command;
        message = receive_message(node.parent);
        std:: istringstream request(message);
        request >> command;
        if (command == "pid") {
            std:: string answer = std:: to_string(getpid());
            send_message(node.parent, answer);
        }
        else if (command == "ping") {
            int child;
            request >> child;
            std:: string answer = node.ping(child);
            send_message(node.parent, answer);
        }
        else if (command == "create") {
            int child;
            request >> child;
            std:: string answer = node.create(child);
            send_message(node.parent, answer);
        }
        else if (command == "send"){
            int child;
            std:: string str;
            request >> child;
            getline(request, str);
            str.erase(0, 1);
            std:: string answer = node.sendstring(str, child);
            send_message(node.parent, answer);
        }
        else if (command == "exec") {
            std:: string str;
            getline(request, str);
            std:: string answer = node.exec(str);
            send_message(node.parent, answer);
        }
        else if (command == "kill") {
            std:: string answer = node.kill();
            send_message(node.parent, answer);
            disconnect(node.parent, node.parent_port);
            node.parent.close();
            break;
        }
        else if (command == "clear") {
            int child;
            request >> child;
            std:: string answer = node.treeclear(child);
            send_message(node.parent, answer);
        }
    }
    return 0;
}