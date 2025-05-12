#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "MessageQueue.hpp"

class Receiver {
public:
    Receiver(const std::string& filename, int numRecords)
        : filename_(filename), queue_(filename) {
        queue_.init(numRecords);
    }

    std::string readMessage() {
        auto result = queue_.pop();
        if (result.first) {
            return result.second;
        }
        return "";
    }

private:
    std::string filename_;
    MessageQueue queue_;
};

#endif
