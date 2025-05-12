cmake_minimum_required(VERSION 3.10)
project(MessageQueueFullTest)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# ✅ ФИКС: Задаём динамическую сборку для GoogleTest (важно для MSVC)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(FullTest
    MessageQueueTest.cpp
    ReceiverSenderTest.cpp
    MessageQueue.hpp
    Receiver.hpp
    Sender.hpp
)

add_executable(ReceiverMain
    ReceiverMain.cpp
    MessageQueue.hpp
    Receiver.hpp
)

add_executable(SenderMain
    SenderMain.cpp
    MessageQueue.hpp
    Sender.hpp
)

target_link_libraries(FullTest gtest gtest_main)

if (NOT WIN32)
    target_link_libraries(FullTest pthread)
    target_link_libraries(ReceiverMain pthread)
    target_link_libraries(SenderMain pthread)
endif()

include(GoogleTest)
gtest_discover_tests(FullTest)
