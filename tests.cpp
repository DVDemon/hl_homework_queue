#include <gtest/gtest.h>

#include <time.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
using namespace std::literals::chrono_literals;

TEST(dummy_test, basic_test_set)
{
    ASSERT_TRUE(true);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}