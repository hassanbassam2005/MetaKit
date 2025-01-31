#ifndef HELPER__H
#define HELPER__H

namespace metakit
{
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
    static constexpr bool is_same_v = false;

    /**
     * @brief Specialization for matching types.
     */
    template<typename T1>
    static constexpr bool is_same_v<T1, T1> = true;

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
        using type = typename strip_pointer<T>::type;
    };

    /**
     * @brief Alias for removing pointers from types.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    using strip_pointer_t = typename strip_pointer<T>::type;

    /**
     * @brief Static assertion to ensure `strip_pointer_t<int*>` correctly reduces to `int`.
     */
    static_assert(std::is_same_v<strip_pointer_t<int*>, int>);

    /**
     * @brief Primary template for removing const and volatile qualifiers from a type.
     *
     * Provides a type alias `type` that retains the original type and a nested alias template
     * `no_cv` to apply a transformation while preserving qualifiers.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    struct remove_cv
    {
        using type = T; ///< Alias for the type without qualifiers.

        /**
         * @brief Applies a transformation `Tn` to the type without qualifiers.
         *
         * @tparam Tn The template to apply.
         */
        template<template<typename> class Tn>
        using no_cv = Tn<T>;
    };

    /**
     * @brief Specialization for removing `const` qualifier from a type.
     *
     * Provides a type alias `type` without the `const` qualifier and `no_cv` to apply
     * transformations while retaining the `const` qualifier.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    struct remove_cv<const T>
    {
        using type = T;

        template<template<typename> class Tn>
        using no_cv = const Tn<T>;
    };

    /**
     * @brief Specialization for removing `volatile` qualifier from a type.
     *
     * Provides a type alias `type` without the `volatile` qualifier and `no_cv` to apply
     * transformations while retaining the `volatile` qualifier.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    struct remove_cv<volatile T>
    {
        using type = T;

        template<template<typename> class Tn>
        using no_cv = volatile Tn<T>;
    };

    /**
     * @brief Specialization for removing both `const` and `volatile` qualifiers from a type.
     *
     * Provides a type alias `type` without the `const volatile` qualifiers and `no_cv`
     * to apply transformations while retaining both qualifiers.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    struct remove_cv<const volatile T>
    {
        using type = T;

        template<template<typename> class Tn>
        using no_cv = const volatile Tn<T>;
    };

    /**
     * @brief Alias template for simplifying access to the `type` member of `remove_cv`.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    /**
     * @brief Removes references from types and provides constant reference equivalents.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    struct remove_reference {
        using type = T;            ///< Type without reference.
        using const_ref = const T; ///< Corresponding constant reference type.
    };

    /**
     * @brief Specialization for l-value references (T&).
     */
    template<typename T>
    struct remove_reference<T&> {
        using type = T;
        using const_ref = const T&;
    };

    /**
     * @brief Specialization for r-value references (T&&).
     */
    template<typename T>
    struct remove_reference<T&&> {
        using type = T;
        using const_ref = const T&&;
    };

    /**
     * @brief Alias for accessing the type without references.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    using remove_refernce_t = typename remove_reference<T>::type;

    /**
     * @brief Alias for getting a constant reference type after removing references.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    using const_through_ref = typename remove_reference<T>::const_ref;

    /**
     * @brief Removes references and const/volatile qualifiers from types.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    using Remove_cvrf_t = typename remove_cv<remove_refernce_t<T>>::type;

    /**
     * @brief Alias for consistent naming in removing references and qualifiers.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    using remove_cvrf_t = Remove_cvrf_t<T>;

    /**
     * @brief Checks if a type is const.
     *
     * This is a compile-time check that returns `true` for const types,
     * and `false` otherwise.
     */
    template<typename>
    constexpr bool is_const_v = false;

    /**
     * @brief Specialization for const types.
     *
     * This specialization sets `is_const_v` to `true` when the type is `const T`.
     */
    template<typename T>
    constexpr bool is_const_v<const T> = true;

    /**
     * @brief Checks if a type is an l-value reference.
     *
     * This is a compile-time check that returns `true` for l-value references (`T&`),
     * and `false` otherwise.
     */
    template<typename>
    constexpr bool is_lvalue_reference_v = false;

    /**
     * @brief Specialization for l-value reference types.
     *
     * This specialization sets `is_lvalue_reference_v` to `true` when the type is
     * an l-value reference (`T&`).
     */
    template<typename T>
    constexpr bool is_lvalue_reference_v<T&> = true;

    /**
     * @brief Trait to determine if a type is an l-value reference.
     *
     * This trait wraps `is_lvalue_reference_v` in a `bool_constant` to provide a
     * more standard interface for use in other template code.
     */
    template<typename T>
    struct is_lvalue_reference : bool_constant<is_lvalue_reference_v<T>> {};

    /**
     * @brief Trait to detect rvalue references at compile time.
     *
     * This template specializes to `true` for rvalue references (T&&),
     * and `false` for other types.
     *
     * @tparam T The type to check.
     */
    template<typename>
    constexpr bool is_rvalue_reference_v = false;

    /**
     * @brief Specialization of `is_rvalue_reference_v` for rvalue references.
     *
     * This specialization sets the value of `is_rvalue_reference_v` to `true`
     * when the type is an rvalue reference (T&&).
     *
     * @tparam T The type to check, specifically for T&&.
     */
    template<typename T>
    constexpr bool is_rvalue_reference_v<T&&> = true;

    /**
     * @brief A type trait to check if a type is an rvalue reference.
     *
     * This structure inherits from `bool_constant`, providing a static constant
     * value indicating whether the type T is an rvalue reference. It uses the
     * `is_rvalue_reference_v` helper variable for the check.
     *
     * @tparam T The type to check.
     */
    template<typename T>
    struct is_rvalue_reference : bool_constant<is_rvalue_reference_v<T>> {};

    /**
     * @brief Forwarding function for lvalue references.
     *
     * This function casts the given lvalue to an rvalue reference of the
     * appropriate type. It is used for perfectly forwarding lvalues.
     *
     * @tparam T The type of the argument.
     * @param args The lvalue to forward.
     * @return The forwarded lvalue as an rvalue reference.
     */
    template<typename T>
    constexpr T&& forward(remove_refernce_t<T>& args) noexcept
    {
        return static_cast<T&&>(args);
    }

    /**
     * @brief Forwarding function for rvalue references.
     *
     * This function casts the given rvalue to an rvalue reference of the
     * appropriate type. It is used for perfectly forwarding rvalues.
     *
     * @tparam T The type of the argument.
     * @param args The rvalue to forward.   
     * @return The forwarded rvalue as an rvalue reference.
     */
    template<typename T>
    requires(is_rvalue_reference_v<T>)//added in c++20
    constexpr T&& forward(remove_refernce_t<T>&& args) noexcept
    {
        return static_cast<T&&>(args);
    }

    /**
     * @brief Moves an object to an rvalue reference, enabling efficient transfer of ownership.
     *
     * @tparam T The type of the object to be moved.
     * @param args The object to be moved.
     * @return An rvalue reference to the object, enabling ownership transfer.
     */
    template<typename T>
    constexpr remove_refernce_t<T>&& move(T&& args) noexcept
    {
        return static_cast<remove_refernce_t<T>&&>(args);
    }


    /**
     * @brief Checks if a type T is equal to any of the types in Ts.
     *
     * This is a variadic template that compares the type T with each of the types
     * in the parameter pack Ts. It returns true if T matches any of the types in Ts.
     */
    template<typename T, typename...Ts>
    constexpr bool is_any_of = (is_same_v<T, Ts> || ...);

    /**
     * @brief Checks if a type T is an integral type.
     *
     * This is a specialization of the `is_any_of` template to check if T is any of
     * the predefined integral types such as bool, char, int, long, etc.
     */
    template<typename T>
    constexpr bool is_integral_v = is_any_of<remove_cvrf_t<T>, bool, char, signed char, unsigned char, wchar_t, char8_t,
        char16_t, char32_t, short, unsigned short, int, unsigned int,
        long, unsigned long, long long, unsigned long long>;

    /**
     * @brief Type trait to check if a type T is integral.
     *
     * This structure derives from `bool_constant` and uses `is_integral_v<T>` to
     * determine whether the type T is an integral type. The result is a constant
     * boolean value, either true or false.
     */
    template<typename T>
    struct is_integral : bool_constant<is_integral_v<T>> {};

    /** 
    * @brief A structure to represent a sequence of integral values as a compile-time type.
    *
    * This template represents a sequence of integral values (of type `T`) and provides a utility to retrieve
    * the size of the sequence.
     *
     * @tparam T The integral type of the values in the sequence.
    * @tparam Ts The values of the sequence.
    */
    template<typename T,T...Ts>  
    requires(is_integral_v<T>)///< Ensures that T is an integral type (e.g., int, char, etc.)
    struct integer_sequence
    {
        using type = T; ///< The type of the sequence elements (must be integral).

        /**
         * @brief Retrieves the size of the sequence.
         *
         * @return The number of elements in the sequence (the number of `Ts`).
         */
        static constexpr size_t size() noexcept
        {
            return sizeof...(Ts);  ///< Returns the number of arguments (elements) in the parameter pack `Ts`.
        }
    };


	/**
	 * @brief Alias template for creating an integer sequence.
	 *
	 * @tparam T The integral type of the sequence.
	 * @tparam Ts The values in the sequence.
	 */
    template<typename T, T Size>
    using make_integer_sequence = __make_integer_seq<integer_sequence,T, Size>;

	/**
	 * @brief Alias template for creating an integer sequence.
	 *
	 * @tparam N The number of elements in the sequence.
	 */
    template <size_t N>
    using make_index_sequence = make_integer_sequence<size_t, N>;

    /**
     * @brief Represents an index sequence of size_t values.
     *
     * @tparam Ts The values in the index sequence.
     */
    template <size_t... Ts>
    using index_sequence = integer_sequence<size_t, Ts...>;


   template<int First,int Last,typename lambda>
   constexpr void static_for(const lambda& f)
   {
       if constexpr(First<Last)
       {
           f(integral_constant<int, First>{});
           static_for<First + 1, Last>(f);
       }
   }
}

#endif
