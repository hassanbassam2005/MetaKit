#ifndef TUPLE_H
#define TUPLE_H

#include "type_list.h"

#ifndef HELPER__H
#include "helper_.h"
#endif 

using namespace helper_;

namespace tup
{
    /**
     * @brief Base case for an empty tuple.
     *
     * Defines an empty tuple structure for recursive tuple definition.
     */
    template<typename... elements>
    struct tuple { constexpr tuple() = default; };

    /**
     * @brief Recursive tuple definition to store a sequence of elements.
     *
     * @tparam element1 The first element type in the tuple.
     * @tparam element2... Remaining element types in the tuple.
     */
    template<typename element1, typename ... element2>
    struct tuple<element1, element2...> : tuple<element2...>
    {
        /**
         * @brief Constructs a tuple with the given elements.
         *
         * @param e1 The first element in the tuple.
         * @param rest The remaining elements in the tuple.
         */
        template<typename T,typename ... Ts>
        explicit constexpr tuple(T&& e1, Ts&&... rest)
            : tuple<element2...>(forward<Ts&&>(rest)...), data(forward<T>(e1)) {}

        element1 data; //Stores the data for the current tuple element.
    };

    /**
     * @brief Deduction guide for template argument deduction for tuple constructors.
     *
     * @tparam T The first element type.
     * @tparam Ts... Remaining element types.
     */
    template<typename T, typename ...Ts>
    tuple(T e1, Ts... e2) -> tuple<std::unwrap_ref_decay_t<T>, std::unwrap_ref_decay_t<Ts>...>;

    /**
     * @brief Factory function to create a tuple.
     *
     * @param elem The elements to be included in the tuple.
     * @return A tuple containing the provided elements.
     */
    template<typename ... elements>
    constexpr auto make_tuple(elements&&... elem)
    {
        return tuple<std::unwrap_ref_decay_t<elements>...>{forward<elements>(elem)...};
    }

    namespace detail
    {
        /**
         * @brief Recursive implementation for accessing tuple elements by index.
         *
         * @tparam i The index of the element to access.
         * @tparam Tuple The tuple type being accessed.
         */
        template<size_t i, typename Tuple>
        struct get_impl : get_impl<i - 1, pop_front_t<Tuple>> {};

        /**
         * @brief Specialization for accessing the first element of a tuple.
         *
         * @tparam Tuple The tuple type being accessed.
         */
        template <typename Tuple>
        struct get_impl<0, Tuple>
        {
            /**
             * @brief Retrieves the element at the specified index from the tuple.
             *
             * @tparam T The type of the tuple instance.
             * @param t The tuple instance from which the element will be retrieved.
             * @return The element at the specified index, adjusted for reference and const-ness.
             */
            template<typename T>
            constexpr static decltype(auto) get(T&& t)
            {
                constexpr bool is_lval = is_lvalue_reference_v<T>;
                constexpr bool is_constant = is_const_v<remove_refernce_t<T>>;

                using data_t = front_t<Tuple>;

                if constexpr (is_lval && is_constant)
                    return static_cast<const data_t&>(static_cast<const Tuple&>(t).data);//lvalue and constant
                if constexpr (is_lval && !is_constant)
                    return static_cast<data_t&>(static_cast<Tuple&>(t).data);//lvalue and not constant
                if constexpr (!is_lval && is_constant)
                    return static_cast<const data_t&&>(static_cast<const Tuple&&>(t).data);//rvalue and constant
                if constexpr (!is_lval && !is_constant)
                    return static_cast<data_t&&>(static_cast<Tuple&&>(t).data);//rvalue and not constant
            }
        };

        struct tuple_cat_impl
        {
            template<typename Tuples>
            struct tuple_size;

            template<typename ... elements>
            struct tuple_size<tuple<elements... >> : integral_constant<size_t, sizeof...(elements)> {};

            template<typename Tuples>
            static constexpr size_t tuple_size_v = tuple_size<Tuples>::value;

            template<typename Tuple1, typename Tuple2>
            static auto f(Tuple1&& t1, Tuple2&& t2)
            {
                return cat_from_indices(std::forward<Tuple1>(t1),
                    std::forward<Tuple2>(t2),
                    std::make_index_sequence<tuple_size_v<remove_cvrf_t<Tuple1>>>{},
                    std::make_index_sequence<tuple_size_v<remove_cvrf_t<Tuple2>>>{});
            }

            template<typename Tuple1, typename Tuple2, size_t ...indices1, size_t ...indices2>
            static auto cat_from_indices(Tuple1&& tuple1,
                Tuple2&& tuple2,
                std::index_sequence<indices1...>,
                std::index_sequence<indices2...>)
            {
                return tuple{ get<indices1>(forward<Tuple1>(tuple1))...,get<indices2>(forward<Tuple2>(tuple2))... };
            }
        };
    }//end of namespace detail

    /**
     * @brief Retrieves the element at the specified index in the tuple.
     *
     * @tparam i The index of the element.
     * @tparam Tuple The tuple type being accessed.
     * @param tuple The tuple instance.
     * @return The element at the specified index.
     */
    template<size_t i, typename Tuple>
    constexpr decltype(auto) get(Tuple&& tuple)
    {
        return detail::get_impl<i, remove_cvrf_t<Tuple>>::get(forward<Tuple>(tuple));
    }

    template<typename ... Tuple>
    constexpr decltype(auto) tuple_cat(Tuple&&... tuples)
    {
        return detail::tuple_cat_impl::f(forward<Tuple>(tuples)...);
    }
}

#endif
