#include "testCopying.cpp"
#include <tuple>

using namespace metakit;
using namespace test;

int main()
{
	constexpr size_t metakit_tuple = 1;
	constexpr size_t std_tuple = 2;

    testing::Tester::test("constructor", []()
        {
            /**
             * @brief Tests the constructor behavior of metakit_tuple and std::tuple.
             *
             * This test ensures that both `metakit_tuple` and `std::tuple` have the same copy/move
             * characteristics when initialized with integer, custom copy-counter, and string values.
             */
            auto c1 = make_copy_counter<metakit_tuple>(); ///< Creates a copy counter for metakit_tuple.
            auto c2 = make_copy_counter<std_tuple>(); ///< Creates a copy counter for std_tuple.

            tuple t1{ 1, c1, "hassan" }; ///< Constructs a metakit tuple.
            std::tuple t2{ 1, c2, "bassam" }; ///< Constructs a standard tuple.

            ASSERT_EQ(c1, c2); ///< Ensures that copy/move statistics are identical.
        });

    testing::Tester::test("make_tuple", []()
        {
            /**
             * @brief Tests the `make_tuple` function for metakit and std::tuple.
             *
             * This test verifies that `make_tuple` from metakit and `std::make_tuple`
             * have equivalent copy/move behavior.
             */
            auto c1 = make_copy_counter<metakit_tuple>(); ///< Creates a copy counter for metakit_tuple.
            auto c2 = make_copy_counter<std_tuple>(); ///< Creates a copy counter for std_tuple.

            [[maybe_unused]] auto&& t1 = make_tuple(1, c1, "hassan"); ///< Constructs a metakit tuple using `make_tuple`.
            [[maybe_unused]] auto&& t2 = std::make_tuple(1, c2, "hassan"); ///< Constructs a standard tuple using `std::make_tuple`.

            ASSERT_EQ(c1, c2); ///< Validates that the copy/move characteristics match.
        });



	return 0;
}

