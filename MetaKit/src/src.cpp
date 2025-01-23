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
			auto c1 = make_copy_counter<metakit_tuple>();
			auto c2 = make_copy_counter<std_tuple>();

			tuple t1 {1, c1, 4};
			std::tuple t2 { 1, c2, 4 };
	
			ASSERT_EQ(c1, c2 );
		});

	return 0;
}

