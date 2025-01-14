#ifndef TUPLE_H
#define TUPLE_H

#include "type_list.h"

#ifndef HELPER__H
#include "helper_.h"
#endif 

using namespace metakit;

namespace metakit
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

        /**
         * @brief Helper struct to concatenate multiple tuples.
         *
         * This struct contains utilities to compute tuple sizes and perform
         * tuple concatenation by forwarding elements from two tuples.
         */
        struct tuple_cat_impl
        {
            /**
             * @brief Helper to compute the size of a given tuple.
             *
             * This specialization determines the number of elements in a tuple.
             *
             * @tparam Tuples The tuple type whose size is being computed.
             */
            template<typename Tuples>
            struct tuple_size;

            /**
             * @brief Specialization for computing the size of a tuple.
             *
             * This uses `sizeof...` to determine the number of elements in a tuple.
             *
             * @tparam elements Parameter pack representing tuple elements.
             */
            template<typename ... elements>
            struct tuple_size<tuple<elements...>> : integral_constant<size_t, sizeof...(elements)> {};

            /**
             * @brief Compile-time constant for the size of a tuple.
             *
             * @tparam Tuples The tuple type whose size is being computed.
             */
            template<typename Tuples>
            static constexpr size_t tuple_size_v = tuple_size<Tuples>::value;

            template<typename ...T>
            static constexpr tuple<T&&...> forward_as_tuple(T&&... args)
            {
                return tuple<T&&...>(forward<T>(args)...);
            };

            /**
             * @brief Concatenates two tuples by forwarding their elements.
             *
             * This function uses index sequences to access and concatenate tuple elements.
             *
             * @tparam Tuple1 The type of the first tuple.
             * @tparam Tuple2 The type of the second tuple.
             * @param t1 The first tuple.
             * @param t2 The second tuple.
             * @return The concatenated tuple.
             */
            template<typename rest_tuple ,typename Tuple, typename... Tuples>
            static constexpr auto f(rest_tuple&& rest,Tuple&& t, Tuples&&... ts)
            {
                return cat_from_indices(
                    forward<rest_tuple>(rest),
                    forward<Tuple>(t),
                    std::make_index_sequence<tuple_size_v<remove_cvrf_t<rest_tuple>>>{},
                    std::make_index_sequence<tuple_size_v<remove_cvrf_t<Tuple>>>{}
                );
            }

            template<typename rest_tuple>
            static constexpr auto f(rest_tuple&& rest)
            {
                return forward<rest_tuple>(rest);
            }
            /**
             * @brief Helper function to concatenate tuples using index sequences.
             *
             * This function accesses tuple elements using indices and constructs
             * a new concatenated tuple by forwarding the elements.
             *
             * @tparam Tuple1 The type of the first tuple.
             * @tparam Tuple2 The type of the second tuple.
             * @tparam indices1 The index sequence for the first tuple.
             * @tparam indices2 The index sequence for the second tuple.
             * @param tuple1 The first tuple.
             * @param tuple2 The second tuple.
             * @return The concatenated tuple.
             */
            template<typename Tuple1, typename Tuple2, size_t ...indices1, size_t ...indices2>
            static constexpr auto cat_from_indices(Tuple1&& tuple1,
                Tuple2&& tuples,
                std::index_sequence<indices1...>,
                std::index_sequence<indices2...>)
            {
                return tuple{ get<indices1>(forward<Tuple1>(tuple1))...,get<indices2>(forward<Tuple2>(tuples))... };
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

    /**
     * @brief Concatenates multiple tuples into a single tuple.
     *
     * This function forwards the given tuples to a helper function
     * in `detail::tuple_cat_impl` to perform the concatenation.
     *
     * @tparam Tuple Parameter pack representing the types of the input tuples.
     * @param tuples The tuples to concatenate.
     * @return The concatenated tuple.
     */
    template<typename ... Tuple>
    constexpr decltype(auto) tuple_cat(Tuple&&... tuples)
    {
        return detail::tuple_cat_impl::f(forward<Tuple>(tuples)...);
    }

}

#endif
