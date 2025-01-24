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
         * @brief Primary template for converting an index sequence into a tuple using a forward tuple.
         *
         * This template is left undefined and is specialized for handling specific index sequences.
         *
         * @tparam index The index sequence type (`std::index_sequence`).
         */
        template <typename index>
        struct make_tuple_from_fwd_tuple;

        /**
         * @brief Specialization for constructing a tuple from a forwarded tuple using an index sequence.
         *
         * Extracts elements from the forwarded tuple (`fwd_tuple`) at the specified indices and constructs a new tuple.
         *
         * @tparam indices The index sequence for selecting elements from the forwarded tuple.
         */
        template <size_t... indices>
        struct make_tuple_from_fwd_tuple<index_sequence<indices...>> {
            /**
             * @brief Constructs a new tuple by extracting elements at the specified indices from the forwarded tuple.
             *
             * @tparam fwd_tuple The forwarded tuple type.
             * @param fwd The forwarded tuple instance.
             * @return A new `tuple` containing the extracted elements.
             */
            template <typename fwd_tuple>
            static constexpr auto f(fwd_tuple&& fwd) {
                return tuple{ get<indices>(forward<fwd_tuple>(fwd))... };
            }
        };

        /**
         * @brief Constructs a tuple from perfectly forwarded arguments.
         *
         * Each argument is stored as an rvalue reference in the tuple, preserving its forwarding status.
         *
         * @tparam T The types of the arguments.
         * @param args The arguments to be forwarded into the tuple.
         * @return A tuple containing rvalue references to the arguments.
         */
        template<typename ...T>
        static constexpr tuple<T&&...> forward_as_tuple(T&&... args)
        {
            return tuple<T&&...>(forward<T>(args)...);
        };

        /**
         * @brief Primary template for concatenating two tuples while preserving forwarding.
         *
         * This template is left undefined and is specialized for handling specific index sequences.
         *
         * @tparam FWD_INDEX_SEQ The index sequence for elements from the forwarded tuple.
         * @tparam TUPLE_INDEX_SEQ The index sequence for elements from the existing tuple.
         */
        template <typename FWD_INDEX_SEQ, typename TUPLE_INDEX_SEQ>
        struct concat_with_fwd_tuple;

        /**
         * @brief Specialization for merging two tuples while preserving forwarding.
         *
         * Constructs a new tuple by extracting elements from a forwarded tuple (`fwd_tuple`) and an existing tuple (`Tuple`).
         * The forwarding ensures perfect preservation of value categories.
         *
         * @tparam fwd_indices The index sequence for selecting elements from the forwarded tuple.
         * @tparam indices The index sequence for selecting elements from the existing tuple.
         */
        template <size_t... fwd_indices, size_t... indices>
        struct concat_with_fwd_tuple<index_sequence<fwd_indices...>, index_sequence<indices...>> {
            /**
             * @brief Combines a forwarded tuple and another tuple into a single tuple while preserving forwarding.
             *
             * @tparam fwd_tuple The type of the forwarded tuple.
             * @tparam Tuple The type of the existing tuple.
             * @param fwd The forwarded tuple.
             * @param t The existing tuple.
             * @return A new tuple containing elements from both input tuples.
             */
            template <typename fwd_tuple, typename Tuple>
            static constexpr auto f(fwd_tuple&& fwd, Tuple&& t) {
                return forward_as_tuple(get<fwd_indices>(forward<fwd_tuple>(fwd))...,
                    get<indices>(forward<Tuple>(t))...);
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
            static constexpr auto f(rest_tuple&& rest,Tuple&& t,Tuples&&... ts)
            {
                return f(concat_with_fwd_tuple<
                    make_index_sequence<tuple_size_v<remove_cvrf_t<rest_tuple>>>,
                    make_index_sequence<tuple_size_v<remove_cvrf_t<Tuple>>>>::f(forward<rest_tuple>(rest),
                                                                                     forward<Tuple>(t)),
                    forward<Tuples>(ts)...);
            }

            template<typename fwd_tuple>
            static constexpr auto f(fwd_tuple && rest)
            {
                return make_tuple_from_fwd_tuple<make_index_sequence<tuple_size_v<fwd_tuple>>>::f(forward<fwd_tuple>(rest));
            }
        };


        /**
         * @brief Concatenates the contents of a tuple, flattening nested tuples.
         *
         * @tparam Tup The type of the input tuple containing potentially nested tuples.
         * @tparam indecise Index sequence representing tuple element positions.
         * @param T The input tuple whose elements will be concatenated.
         * @return A flattened tuple containing all elements from nested tuples.
         */
        template<typename Tup, size_t... indecise>
        constexpr auto cat_tuple_content(Tup&& T, index_sequence<indecise...>)
        {
            return tuple_cat(get<indecise>(forward<Tup>(T))...);
        }


        /**
         * @brief Applies a transformation function to each element of a tuple (implementation).
         *
         * @tparam Tup The type of the input tuple.
         * @tparam Func The type of the transformation function.
         * @tparam indecise Index sequence representing tuple element positions.
         * @param tup The input tuple to be transformed.
         * @param func The function to apply to each element of the tuple.
         * @return A new tuple with transformed elements.
         */
        template<typename Tup, typename Func, size_t ...indecise>
        constexpr auto transform_impl(Tup&& tup, Func&& func, index_sequence<indecise...>)
        {
            return tuple{ func(get<indecise>(forward<Tup>(tup)))... };
        }


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


    /**
     * @brief Applies a transformation function to each element of a tuple.
     *
     * @tparam Tup The type of the input tuple.
     * @tparam Func The type of the transformation function.
     * @param tup The input tuple to be transformed.
     * @param func The function to apply to each element of the tuple.
     * @return A new tuple with transformed elements.
     */
    template<typename Tup, typename Func>
    constexpr auto transform(Tup&& tup, const Func& func)
    {
        return detail::transform_impl(forward<Tup>(tup), func,
            make_index_sequence<detail::tuple_size_v<remove_cvrf_t<Tup>>>{});
    }
    
    /**
     * @brief Filters elements of a tuple based on a predicate.
     *
     * @tparam Pred A template predicate that determines which elements to keep.
     * @tparam Tup The type of the input tuple.
     * @param t The input tuple to be filtered.
     * @return A new tuple containing only the elements that satisfy the predicate.
     */
    template<template<typename ...> class Pred, typename Tup>
    constexpr auto filter(Tup&& t)
    {
        /**
         * @brief Wraps an element in a tuple if it matches the predicate.
         *
         * @tparam Elem The type of the current element.
         * @param e The element to be checked and potentially wrapped.
         * @return A single-element tuple if the predicate matches, otherwise an empty tuple.
         */
        auto wrap_if_pred_matches = [&]<typename Elem>(Elem && e)
        {
            if constexpr (Pred<remove_cvrf_t<Elem>>::value)
            {
                return forward_as_tuple(forward<Elem>(e));
            }
            else
            {
                return tuple<>{};
            }
        };

        // Apply the wrapping function to each element in the tuple.
        auto wrapped_tuple = transform(forward<Tup>(t), wrap_if_pred_matches);

        /**
         * @brief Concatenates the wrapped tuples into a single tuple, removing empty ones.
         *
         * @return A filtered tuple containing only the elements that satisfy the predicate.
         */
        return detail::cat_tuple_content(move(wrapped_tuple),
            make_index_sequence<detail::tuple_size_v<remove_cvrf_t<Tup>>>{});
    }


    /**
     * @brief Primary template for retrieving the type of an element in a tuple at a given index.
     *
     * This template is left undefined and is specialized for handling specific tuple cases.
     *
     * @tparam i The index of the element to retrieve.
     * @tparam Tuple The tuple type being accessed.
     */
    template<size_t i, typename Tuple>
    struct tuple_element;

    /**
     * @brief Specialization for retrieving the first element (index 0) of the tuple.
     *
     * Extracts the first element type (`Head`) from the tuple.
     *
     * @tparam Head The type of the first element in the tuple.
     * @tparam Tail... The remaining element types in the tuple.
     */
    template<typename Head, typename ...Tail>
    struct tuple_element<0, tuple<Head, Tail...>>
    {
        using type = Head; ///< The type of the first element in the tuple.
    };

    /**
     * @brief Specialization for retrieving the `i`-th element of the tuple.
     *
     * Recursively accesses the `i-1` index of the remaining tuple elements.
     *
     * @tparam i The index of the element to retrieve.
     * @tparam Head The first element type in the tuple.
     * @tparam Tail... The remaining element types in the tuple.
     */
    template<size_t i, typename Head, typename ...Tail>
    struct tuple_element<i, tuple<Head, Tail...>>
    {
        using type = typename tuple_element<i - 1, tuple<Tail...>>::type; ///< The type of the `i`-th element.
    };

    /**
     * @brief Alias template for simplifying access to the `type` member of `tuple_element`.
     *
     * @tparam i The index of the element.
     * @tparam Tuple The tuple type being accessed.
     */
    template<size_t i, typename Tuple>
    using tuple_element_t = typename tuple_element<i, Tuple>::type;


}

#endif
