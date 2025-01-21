#include "testCopying.cpp"
#include <tuple>

using namespace metakit;
using namespace test;

int main()
{
	auto tup = make_tuple(1,2.4,"hassan");
	std::cout << get<2>(tup) << '\n';

	const tuple const_tup{ 1, false, 4 };
	std::cout << get<2>(const_tup) << '\n';


	std::cout << get<2>(tuple{ 2, 3, 8 }) << '\n';
	get<1>(tup) = 4.2;
	std::cout << get<2>(tup) << '\n';
	
	CopyCounter c;
	CopyStats M_stat = c.stats;


	std::cout << "metakit::make_tuple stats: " <<M_stat << '\n';
	
	CopyCounter::reset();

	tuple tup3{ 1,c,3,"hassan" };
	tuple tup4{ 3.4,"basssam" };

	auto to = tuple_cat(tup3, tup4);

	std::cout << "metakit::tuple_cat stats: " << c.stats << '\n';
	CopyCounter::reset();

	auto to2 = std::tuple_cat(std::tuple{ 1, c, 3, "hassan" }, std::tuple{ 3.4,"basssam" });

	std::cout << "std::tuple_cat stats: " << c.stats << '\n';
	CopyCounter::reset();

	return 0;
}

