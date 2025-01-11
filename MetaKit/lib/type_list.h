#ifndef HEADER_H
#define HEADER_H

#include <string>

#ifndef HELPER__H
#include "helper_.h"
#endif

using namespace helper_;

/**
 * @brief Represents a variadic template type list.
 */
template <typename...>
struct type_list {};

/**
 * @brief Trait to check if a type list is empty.
 *
 * @tparam LIST The type list to check.
 */
template <typename LIST>
struct empty : false_type {};

/**
 * @brief Specialization for empty type list.
 */
template <template <typename...> class LIST>
struct empty<LIST<>> : true_type {};

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
struct pop_front<LIST<T0, T1toN...>> : has_type<LIST<T1toN...>> {};

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

static_assert(is_same_v<pop_back_t<type_list<int>>, type_list<>>);
static_assert(is_same_v<pop_back_t<type_list<int, bool, float>>, type_list<int, bool>>);
static_assert(is_same_v<pop_back_t<type_list<int, bool>>, type_list<int>>);

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

static_assert(is_same_v<at_t<type_list<int, bool, float>, 1>, bool>);
static_assert(is_same_v<at_t<type_list<int, bool, float>, 2>, float>);

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