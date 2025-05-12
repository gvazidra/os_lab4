#include "gtest/gtest.h"
#include "MessageQueue.hpp"
#include <cstdio>
#include <fstream>

class MessageQueueTest : public ::testing::Test {
protected:
    std::string testFile = "test_queue.bin";
    MessageQueue* queue;

    void SetUp() override {
        queue = new MessageQueue(testFile);
        queue->init(3); 
    }

    void TearDown() override {
        delete queue;
        std::remove(testFile.c_str());
    }
};

TEST_F(MessageQueueTest, InitFileCreatesCorrectStructure) {
    std::ifstream ifs(testFile, std::ios::binary);
    ASSERT_TRUE(ifs.is_open());

    QueueHeader header;
    ifs.read(reinterpret_cast<char*>(&header), sizeof(header));
    EXPECT_EQ(header.head, 0);
    EXPECT_EQ(header.tail, 0);
    EXPECT_EQ(header.capacity, 3);

    ifs.close();
}

TEST_F(MessageQueueTest, PushAndPopSingleMessage) {
    std::string msg = "Hello World";
    bool pushed = queue->push(msg);
    ASSERT_TRUE(pushed);

    auto result = queue->pop();
    ASSERT_TRUE(result.first);
    EXPECT_EQ(result.second.substr(0, msg.size()), msg);
}

TEST_F(MessageQueueTest, PopFromEmptyQueue) {
    auto result = queue->pop();
    ASSERT_FALSE(result.first);
}

TEST_F(MessageQueueTest, PushToFullQueueFails) {
    ASSERT_TRUE(queue->push("Msg1"));
    ASSERT_TRUE(queue->push("Msg2"));
    ASSERT_FALSE(queue->push("Msg3"));
}

TEST_F(MessageQueueTest, FIFOOrderPreserved) {
    queue->push("FirstMsg");
    queue->push("SecondMsg");

    auto first = queue->pop();
    ASSERT_TRUE(first.first);
    EXPECT_EQ(first.second.substr(0, 8), "FirstMsg");

    auto second = queue->pop();
    ASSERT_TRUE(second.first);
    EXPECT_EQ(second.second.substr(0, 9), "SecondMsg");
}
