#include "Receiver.hpp"
#include <iostream>
#include <windows.h>

int main() {
    std::string filename;
    int numRecords, numSenders;

    std::cout << "Enter binary file name: ";
    std::cin >> filename;
    std::cout << "Enter number of records: ";
    std::cin >> numRecords;
    std::cout << "Enter number of Sender processes: ";
    std::cin >> numSenders;

    std::vector<HANDLE> readyEvents;

    for (int i = 0; i < numSenders; ++i) {
        std::string eventName = "Global\\SenderReadyEvent_" + std::to_string(i);
        HANDLE evt = CreateEventA(NULL, TRUE, FALSE, eventName.c_str());
        if (!evt) {
            std::cerr << "Failed to create event: " << eventName << "\n";
        }
        readyEvents.push_back(evt);
    }

    Receiver receiver(filename, numRecords);

    for (int i = 0; i < numSenders; ++i) {
        STARTUPINFOA si = { sizeof(STARTUPINFOA) };
        PROCESS_INFORMATION pi;
        std::string command = "SenderMain.exe " + filename + " " + std::to_string(i);

        if (!CreateProcessA(
            NULL,
            &command[0], 
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi))
        {
            std::cerr << "Failed to start Sender process.\n";
        }
        else {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }

    std::cout << "Waiting for all Senders to be ready...\n";
    WaitForMultipleObjects(readyEvents.size(), readyEvents.data(), TRUE, INFINITE);
    std::cout << "All Senders are ready.\n";

    while (true) {
        std::cout << "Enter command (read/exit): ";
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "read") {
            std::string msg = receiver.readMessage();
            if (msg.empty()) {
                std::cout << "Queue empty, waiting...\n";
            }
            else {
                std::cout << "Received: " << msg << "\n";
            }
        }
        else if (cmd == "exit") {
            break;
        }
    }

    for (HANDLE evt : readyEvents) {
        CloseHandle(evt);
    }

    return 0;
}
