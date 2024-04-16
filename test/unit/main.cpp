//! @file main.cpp
//! @author ryftchen
//! @brief The definitions (main) in the test module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include <gtest/gtest.h>

//! @brief The test module.
namespace test
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief The run function.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
static int run(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return ::RUN_ALL_TESTS();
}
} // namespace
} // namespace test

//! @brief The main function of the unit test.
//! @param argc - argument count
//! @param argv - argument vector
//! @return the argument to the implicit call to exit()
int main(int argc, char* argv[])
{
    return test::run(argc, argv);
}
