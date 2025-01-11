#include "tuple.h"

using namespace tup;

int main()
{
	std::tuple<int, double, bool> list;
	std::list<int> lister;
	type_list<int, double, bool>  l;
	auto tupe = make_tuple(1.4, 5, "hassan");
	tuple tot{ 3,"bassam",2.4 };
	const tuple tupa{ 1.3,"hi" };
	using l1 = type_list<int, float, bool>;
	using l2 = push_back_t<l1, std::string>;

	static_assert(is_same_v<typename front_t<decltype(l)>, int>);
	back_t<decltype(list)> ls;
	std::cout << std::boolalpha << is_same_v<decltype(l), decltype(ls)> << "\n";
	front_t<decltype(lister)> front_list;
	std::cout << std::boolalpha << is_same_v<at_t<decltype(list), 1>, at_t<decltype(l), 1>> << "\n";
	std::cout << is_same_v<front_t<decltype(list)>, front_t<decltype(tupe)>> << "\n";
	std::cout << tupe.data << "\n";
	std::cout << get<1>(tot)<<"\n";
	std::cout << get<1>(tupa)<<"\n";
	return 0;
}
