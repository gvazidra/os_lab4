#include "Sender.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: SenderMain <filename> <senderIndex>\n";
        return 1;
    }

    std::string filename = argv[1];
    int senderIndex = std::stoi(argv[2]);

    Sender sender(filename);

    std::string eventName = "Global\\SenderReadyEvent_" + std::to_string(senderIndex);
    HANDLE evt = OpenEventA(EVENT_MODIFY_STATE, FALSE, eventName.c_str());
    if (evt) {
        SetEvent(evt);
        CloseHandle(evt);
    }
    else {
        std::cerr << "Failed to signal readiness.\n";
    }

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
