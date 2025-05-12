#ifndef SENDER_HPP
#define SENDER_HPP

#include "MessageQueue.hpp"

class Sender {
public:
    Sender(const std::string& filename)
        : filename_(filename), queue_(filename) {
    }

    bool sendMessage(const std::string& msg) {
        return queue_.push(msg);
    }

private:
    std::string filename_;
    MessageQueue queue_;
};

#endif