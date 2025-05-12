#include "Sender.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: SenderMain <filename>\n";
        return 1;
    }
    std::string filename = argv[1];

    Sender sender(filename);

    while (true) {
        std::cout << "Enter command (send/exit): ";
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "send") {
            std::cout << "Enter message (max 20 chars): ";
            std::string msg;
            std::cin.ignore();
            std::getline(std::cin, msg);

            if (msg.size() > 20) msg = msg.substr(0, 20);
            if (sender.sendMessage(msg)) {
                std::cout << "Message sent successfully.\n";
            }
            else {
                std::cout << "Queue is full, message not sent.\n";
            }
        }
        else if (cmd == "exit") {
            break;
        }
    }
    return 0;
}
