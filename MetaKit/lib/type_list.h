#ifndef HEADER_H
#define HEADER_H

#include <string>

#include "helper_.h"

using namespace helper_;

/**
 * @brief Represents a variadic template type list.
 */
template <typename...>
struct type_list {};

/**
 * @brief Trait to check if a type list is empty.
 *
 * @tparam list The type list to check.
 */
template <typename list>
struct empty : false_type {};

/**
 * @brief Specialization for empty type list.
 */
template <template <typename...> class list>
struct empty<list<>> : true_type {};

/**
 * @brief Boolean constant to check if a type list is empty.
 */
template <typename list>
static constexpr bool empty_v = empty<list>::value;

static_assert(empty_v<type_list<>>);
static_assert(empty_v<type_list<int, bool>> == false);

/**
 * @brief Trait to get the first element of a type list.
 */
template <typename list>
struct front;

/**
 * @brief Specialization to extract the first element.
 */
template <template <typename...> class list, typename T1, typename... T2>
struct front<list<T1, T2...>> : has_type<T1> {};

/**
 * @brief Alias to retrieve the first element type.
 */
template <typename list>
using front_t = typename front<list>::type;

static_assert(is_same_v<front_t<type_list<int, bool, float>>, int>);

/**
 * @brief Trait to remove the first type from a type list.
 */
template <typename list>
struct pop_front;

/**
 * @brief Specialization to pop the first type.
 */
template <template <typename...> class list, typename T1, typename... T2>
struct pop_front<list<T1, T2...>> : has_type<list<T2...>> {};

/**
 * @brief Alias to pop the first type from a type list.
 */
template <typename list>
using pop_front_t = typename pop_front<list>::type;

static_assert(is_same_v<pop_front_t<type_list<int, bool, float>>, type_list<bool, float>>);

/**
 * @brief Trait to get the last element of a type list.
 */
template <typename list>
struct back : has_type<typename back<pop_front_t<list>>::type> {};

/**
 * @brief Specialization to extract the last element.
 */
template <template <typename...> class list, typename TL>
struct back<list<TL>> : has_type<TL> {};

/**
 * @brief Alias to retrieve the last element type.
 */
template <typename list>
using back_t = typename back<list>::type;

static_assert(is_same_v<back_t<type_list<int, bool, float>>, float>);
static_assert(is_same_v<back_t<type_list<int, bool>>, bool>);

/**
 * @brief Trait to append a type to the back of a type list.
 */
template <typename list, typename T>
struct push_back;

/**
 * @brief Specialization to push a type at the back.
 */
template <template <typename...> class list, typename... T0, typename T>
struct push_back<list<T0...>, T> : has_type<list<T0..., T>> {};

/**
 * @brief Alias to push a type to the back of a type list.
 */
template <typename list, typename T>
using push_back_t = typename push_back<list, T>::type;

static_assert(is_same_v<push_back_t<type_list<>, int>, type_list<int>>);
static_assert(is_same_v<push_back_t<type_list<int, bool>, float>, type_list<int, bool, float>>);

/**
 * @brief Trait to remove the last type from a type list.
 */
template <typename list, typename Rest_list = type_list<>>
struct pop_back;

/**
 * @brief Specialization for single-element list.
 */
template <template <typename...> class list, typename T0, typename Rest_list>
struct pop_back<list<T0>, Rest_list> : has_type<Rest_list> {};

/**
 * @brief Specialization for lists with multiple elements.
 */
template <template <typename...> class list, typename T0, typename T1, typename... T2, typename Rest_list>
struct pop_back<list<T0, T1, T2...>, Rest_list> : pop_back<list<T1, T2...>, push_back_t<Rest_list, T0>> {};

/**
 * @brief Alias to pop the last type from a type list.
 */
template <typename list>
using pop_back_t = typename pop_back<list>::type;

static_assert(is_same_v<pop_back_t<type_list<int>>, type_list<>>);
static_assert(is_same_v<pop_back_t<type_list<int, bool, float>>, type_list<int, bool>>);
static_assert(is_same_v<pop_back_t<type_list<int, bool>>, type_list<int>>);

/**
 * @brief Trait to get the type at a specific index in a type list.
 */
template <typename list, size_t index>
struct at : has_type<typename at<pop_front_t<list>, index - 1>::type> {};

/**
 * @brief Specialization for index 0.
 */
template <typename list>
struct at<list, 0> : has_type<front_t<list>> {};

/**
 * @brief Alias to get the type at a specific index.
 */
template <typename list, size_t index>
using at_t = typename at<list, index>::type;

static_assert(is_same_v<at_t<type_list<int, bool, float>, 1>, bool>);
static_assert(is_same_v<at_t<type_list<int, bool, float>, 2>, float>);

/**
 * @brief Trait to check if any type in a list satisfies a predicate.
 */
template <template <typename> class Pred, typename list>
struct any;

/**
 * @brief Specialization for empty list.
 */
template <template <typename> class Pred, template <typename...> class list>
struct any<Pred, list<>> : false_type {};

/**
 * @brief Specialization for non-empty list.
 */
template <template <typename> class Pred, typename list>
struct any : if_<
    Pred<front_t<list>>::value,
    true_type,
    typename any<Pred, pop_front_t<list>>::type>::type {};

/**
 * @brief Boolean constant to check if any type matches a predicate.
 */
template <template <typename> class Pred, typename list>
static constexpr bool any_v = any<Pred, list>::value;

static_assert(any_v<std::is_integral, type_list<int, double, std::string>>);
static_assert(any_v<std::is_integral, type_list<std::string, double, int>>);
static_assert(!any_v<std::is_integral, type_list<std::string, double, float>>);

/**
 * @brief Predicate to check if types are the same.
 */
template<typename T1>
struct is_same_pred
{
    template<typename T2>
    struct predicate : is_same<T1, T2>::value {};
};

/**
 * @brief Checks if a type is contained in a list.
 */
template<typename search, typename list>
static constexpr bool contains_type_v = any<is_same_pred<search>::template predicate, list>::value;

#endif