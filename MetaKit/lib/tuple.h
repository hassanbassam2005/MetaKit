#ifndef TUPLE_H
#define TUPLE_H

#include <tuple>
#include <memory>
#include <type_traits>
#include <functional>

#ifndef TYPE_LIST_H
#include "type_list.h"
#endif 



namespace tup
{
	template<typename... elements>
	struct tuple { constexpr tuple() = default; };

	template<typename element1, typename ... element2>
	struct tuple<element1, element2...> : tuple < element2...>
	{
		explicit constexpr tuple(element1 e1, element2... rest) : tuple<element2...>(rest...), data(e1) {}
		element1 data;
	};

	//deduction guide for making template argument deduction for constructors work (c++17)
	template<typename T, typename ...Ts>
	tuple(T e1, Ts... e2) -> tuple<std::unwrap_ref_decay_t<T>, std::unwrap_ref_decay_t<Ts>...>;

	template<typename ... elements>
	auto make_tuple(elements... elem)
	{
		return tuple < std::unwrap_ref_decay_t<elements>...> {elem...};
	};

	namespace detail
	{
		template<size_t i, typename tuple>
		struct get_impl : get_impl<i - 1, pop_front_t<tuple>> {};

		template <typename tuple>
		struct get_impl<0, tuple>
		{
			template<typename T>
			constexpr static decltype(auto) get(T& t)
			{
				return static_cast<tuple&>(t).data;
			}
		};
	}

	/*
	*
	* get to get the index of the type of the tuple
	*
	*/
	template<size_t i, typename tuple>
	constexpr decltype(auto) get(tuple& Tuple)
	{
		return detail::get_impl<i, std::remove_cvref_t<tuple>>::get(Tuple);
	}
}



#endif
