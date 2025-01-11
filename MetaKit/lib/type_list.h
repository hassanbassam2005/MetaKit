#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <list>
#include <string>
#include <tuple>
#include <vector>

/**
 * @brief Represents a variadic template type list.
 */
template <typename...>
struct type_list {};

/**
 * @brief Wrapper to hold a type.
 *
 * @tparam T The type to be wrapped.
 */
template <typename T>
struct has_type {
    using type = T;
};

/**
 * @brief Implements compile-time conditional branching.
 *
 * @tparam condition The compile-time condition to evaluate.
 * @tparam THEN The type to use if the condition is true.
 * @tparam ELSE The type to use if the condition is false.
 */
template <bool condition, typename THEN, typename ELSE>
struct if_;

/**
 * @brief Specialization for true condition.
 */
template <typename THEN, typename ELSE>
struct if_<true, THEN, ELSE> : has_type<THEN> {};

/**
 * @brief Specialization for false condition.
 */
template <typename THEN, typename ELSE>
struct if_<false, THEN, ELSE> : has_type<ELSE> {};

static_assert(std::is_same_v<typename if_<(10 > 5), int, bool>::type, int>);
static_assert(std::is_same_v<typename if_<(10 < 5), int, bool>::type, bool>);

/**
 * @brief Represents a constant integral value as a type.
 *
 * @tparam T The type of the integral value.
 * @tparam val The constant value.
 */
template<typename T, T val>
struct integral_constant
{
    static constexpr T value = val;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

/**
 * @brief Specialization for boolean constants.
 *
 * @tparam T Boolean value.
 */
template<bool T>
using bool_constant = integral_constant<bool, T>;

/**
 * @brief Checks if two types are the same.
 */
template <typename, typename>
constexpr bool is_same_v = false;

/**
 * @brief Specialization for matching types.
 */
template<typename T1>
constexpr bool is_same_v<T1, T1> = true;

/**
 * @brief Boolean trait to determine if two types are the same.
 */
template<typename T1, typename T2>
struct is_same : bool_constant<is_same_v<T1, T2>> {};

/**
 * @brief Boolean false type.
 */
using false_type = bool_constant<false>;

/**
 * @brief Boolean true type.
 */
using true_type = bool_constant<true>;

/**
 * @brief Trait to determine if a type is a pointer.
 */
template<typename T>
struct is_pointer
{
    static constexpr bool value = false;
};

/**
 * @brief Specialization for pointer types.
 */
template<typename T>
struct is_pointer<T*>
{
    static constexpr bool value = true;
};

/**
 * @brief Removes pointer from a type.
 */
template<typename T>
struct strip_pointer
{
    using type = T;
};

/**
 * @brief Specialization to strip pointer types.
 */
template<typename T>
struct strip_pointer<T*>
{
    using type = T;
};

/**
 * @brief Trait to check if a type list is empty.
 *
 * @tparam LIST The type list to check.
 */
template <typename LIST>
struct empty : std::false_type {};

/**
 * @brief Specialization for empty type list.
 */
template <template <typename...> class LIST>
struct empty<LIST<>> : std::true_type {};

/**
 * @brief Boolean constant to check if a type list is empty.
 */
template <typename LIST>
static constexpr bool empty_v = empty<LIST>::value;

static_assert(empty_v<type_list<>>);
static_assert(empty_v<type_list<int, bool>> == false);

/**
 * @brief Trait to get the first element of a type list.
 */
template <typename LIST>
struct front;

/**
 * @brief Specialization to extract the first element.
 */
template <template <typename...> class LIST, typename T0, typename... T1toN>
struct front<LIST<T0, T1toN...>> : has_type<T0> {};

/**
 * @brief Alias to retrieve the first element type.
 */
template <typename LIST>
using front_t = typename front<LIST>::type;

static_assert(is_same_v<front_t<type_list<int, bool, float>>, int>);

/**
 * @brief Trait to remove the first type from a type list.
 */
template <typename LIST>
struct pop_front;

/**
 * @brief Specialization to pop the first type.
 */
template <template <typename...> class LIST, typename T0, typename... T1toN>
struct pop_front<LIST<T0, T1toN...>> : has_type<type_list<T1toN...>> {};

/**
 * @brief Alias to pop the first type from a type list.
 */
template <typename LIST>
using pop_front_t = typename pop_front<LIST>::type;

static_assert(is_same_v<pop_front_t<type_list<int, bool, float>>, type_list<bool, float>>);

/**
 * @brief Trait to get the last element of a type list.
 */
template <typename LIST>
struct back : has_type<typename back<pop_front_t<LIST>>::type> {};

/**
 * @brief Specialization to extract the last element.
 */
template <template <typename...> class LIST, typename T0>
struct back<LIST<T0>> : has_type<T0> {};

/**
 * @brief Alias to retrieve the last element type.
 */
template <typename LIST>
using back_t = typename back<LIST>::type;

static_assert(is_same_v<back_t<type_list<int, bool, float>>, float>);
static_assert(is_same_v<back_t<type_list<int, bool>>, bool>);

/**
 * @brief Trait to append a type to the back of a type list.
 */
template <typename LIST, typename T>
struct push_back;

/**
 * @brief Specialization to push a type at the back.
 */
template <template <typename...> class LIST, typename... T0toN, typename T>
struct push_back<LIST<T0toN...>, T> : has_type<LIST<T0toN..., T>> {};

/**
 * @brief Alias to push a type to the back of a type list.
 */
template <typename LIST, typename T>
using push_back_t = typename push_back<LIST, T>::type;

static_assert(is_same_v<push_back_t<type_list<>, int>, type_list<int>>);
static_assert(is_same_v<push_back_t<type_list<int, bool>, float>, type_list<int, bool, float>>);

/**
 * @brief Trait to remove the last type from a type list.
 */
template <typename LIST, typename RET_LIST = type_list<>>
struct pop_back;

/**
 * @brief Specialization for single-element list.
 */
template <template <typename...> class LIST, typename T0, typename RET_LIST>
struct pop_back<LIST<T0>, RET_LIST> : has_type<RET_LIST> {};

/**
 * @brief Specialization for lists with multiple elements.
 */
template <template <typename...> class LIST, typename T0, typename T1, typename... T2toN, typename RET_LIST>
struct pop_back<LIST<T0, T1, T2toN...>, RET_LIST> : pop_back<LIST<T1, T2toN...>, push_back_t<RET_LIST, T0>> {};

/**
 * @brief Alias to pop the last type from a type list.
 */
template <typename LIST>
using pop_back_t = typename pop_back<LIST>::type;

static_assert(std::is_same_v<pop_back_t<type_list<int>>, type_list<>>);
static_assert(std::is_same_v<pop_back_t<type_list<int, bool, float>>, type_list<int, bool>>);
static_assert(std::is_same_v<pop_back_t<type_list<int, bool>>, type_list<int>>);

/**
 * @brief Trait to get the type at a specific index in a type list.
 */
template <typename LIST, size_t index>
struct at : has_type<typename at<pop_front_t<LIST>, index - 1>::type> {};

/**
 * @brief Specialization for index 0.
 */
template <typename LIST>
struct at<LIST, 0> : has_type<front_t<LIST>> {};

/**
 * @brief Alias to get the type at a specific index.
 */
template <typename LIST, size_t index>
using at_t = typename at<LIST, index>::type;

static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 1>, bool>);
static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 2>, float>);

/**
 * @brief Trait to check if any type in a list satisfies a predicate.
 */
template <template <typename> class PREDICATE, typename LIST>
struct any;

/**
 * @brief Specialization for empty list.
 */
template <template <typename> class PREDICATE, template <typename...> class LIST>
struct any<PREDICATE, LIST<>> : false_type {};

/**
 * @brief Specialization for non-empty list.
 */
template <template <typename> class PREDICATE, typename LIST>
struct any : if_<
    PREDICATE<front_t<LIST>>::value,
    true_type,
    typename any<PREDICATE, pop_front_t<LIST>>::type>::type {};

/**
 * @brief Boolean constant to check if any type matches a predicate.
 */
template <template <typename> class PREDICATE, typename LIST>
static constexpr bool any_v = any<PREDICATE, LIST>::value;

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