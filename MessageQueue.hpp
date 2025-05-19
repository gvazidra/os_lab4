#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

const int MESSAGE_SIZE = 20;
constexpr auto MUTEX_NAME = "Global\\MessageQueueMutex";

struct QueueHeader {
    int head;
    int tail;
    int capacity;
};

class MessageQueue {
public:
    explicit MessageQueue(const std::string& filename)
        : filename_(filename) {
        mutex_ = CreateMutexA(NULL, FALSE, MUTEX_NAME);
        if (mutex_ == NULL) {
            throw std::runtime_error("Failed to create or open mutex");
        }
    }

    ~MessageQueue() {
        if (mutex_ != NULL) {
            CloseHandle(mutex_);
        }
    }

    void init(int numRecords) {
        WaitForSingleObject(mutex_, INFINITE);
        std::ofstream ofs(filename_, std::ios::binary | std::ios::trunc);
        QueueHeader header = { 0, 0, numRecords };
        ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
        std::vector<char> empty(MESSAGE_SIZE, 0);
        for (int i = 0; i < numRecords; ++i) {
            ofs.write(empty.data(), MESSAGE_SIZE);
        }
        ofs.close();
        ReleaseMutex(mutex_);
    }

    bool push(const std::string& message) {
        WaitForSingleObject(mutex_, INFINITE);
        std::fstream fs(filename_, std::ios::binary | std::ios::in | std::ios::out);
        QueueHeader header;
        fs.read(reinterpret_cast<char*>(&header), sizeof(header));

        int nextTail = (header.tail + 1) % header.capacity;
        if (nextTail == header.head) {
            fs.close();
            ReleaseMutex(mutex_);
            return false;
        }

        fs.seekp(sizeof(header) + header.tail * MESSAGE_SIZE);
        std::string msg = message;
        if (msg.size() < MESSAGE_SIZE) {
            msg.resize(MESSAGE_SIZE, '\0');
        }
        fs.write(msg.data(), MESSAGE_SIZE);

        header.tail = nextTail;
        fs.seekp(0);
        fs.write(reinterpret_cast<const char*>(&header), sizeof(header));
        fs.close();
        ReleaseMutex(mutex_);
        return true;
    }

    std::pair<bool, std::string> pop() {
        WaitForSingleObject(mutex_, INFINITE);
        std::fstream fs(filename_, std::ios::binary | std::ios::in | std::ios::out);
        QueueHeader header;
        fs.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (header.head == header.tail) {
            fs.close();
            ReleaseMutex(mutex_);
            return { false, "" };
        }

        fs.seekg(sizeof(header) + header.head * MESSAGE_SIZE);
        char buffer[MESSAGE_SIZE + 1] = { 0 };
        fs.read(buffer, MESSAGE_SIZE);

        header.head = (header.head + 1) % header.capacity;
        fs.seekp(0);
        fs.write(reinterpret_cast<const char*>(&header), sizeof(header));
        fs.close();
        ReleaseMutex(mutex_);
        return { true, std::string(buffer) };
    }

private:
    std::string filename_;
    HANDLE mutex_;
};

#endif
