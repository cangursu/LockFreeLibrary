

#include <gtest/gtest.h>


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

//    ::testing::GTEST_FLAG(filter) = "ObjectStack.MT";
//    ::testing::GTEST_FLAG(filter) = "ObjectPool.*";
//    ::testing::GTEST_FLAG(filter) = "ObjectPool.Basic";

    return RUN_ALL_TESTS();
}

