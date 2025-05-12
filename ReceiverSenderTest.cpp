#include "gtest/gtest.h"
#include "Receiver.hpp"
#include "Sender.hpp"
#include <cstdio>

class ReceiverSenderTest : public ::testing::Test {
protected:
    std::string testFile = "test_queue.bin";
    Receiver* receiver;
    Sender* sender;

    void SetUp() override {
        receiver = new Receiver(testFile, 3);
        sender = new Sender(testFile);
    }

    void TearDown() override {
        delete receiver;
        delete sender;
        std::remove(testFile.c_str());
    }
};

TEST_F(ReceiverSenderTest, SendMessageAndReceive) {
    std::string message = "TestMessage";
    ASSERT_TRUE(sender->sendMessage(message));

    std::string received = receiver->readMessage();
    EXPECT_EQ(received.substr(0, message.size()), message);
}

TEST_F(ReceiverSenderTest, ReceiveFromEmptyQueue) {
    std::string received = receiver->readMessage();
    EXPECT_TRUE(received.empty());
}

TEST_F(ReceiverSenderTest, QueueOverflowBehavior) {
    ASSERT_TRUE(sender->sendMessage("Msg1"));
    ASSERT_TRUE(sender->sendMessage("Msg2"));
    ASSERT_FALSE(sender->sendMessage("Msg3"));
}
