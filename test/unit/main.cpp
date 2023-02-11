//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the test module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include <gtest/gtest.h>

//! @brief The main function of the unit test.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}