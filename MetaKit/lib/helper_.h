#ifndef HELPER__H
#define HELPER__H

namespace helper_
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
    using remove_cvrf_t = typename std::remove_cv_t<remove_refernce_t<T>>;

    /**
     * @brief Alias for consistent naming in removing references and qualifiers.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    using Remove_cvrf_t = remove_cvrf_t<T>;

    /**
     * @brief Removes references, const, and volatile qualifiers from types.
     *
     * @tparam T The type to process.
     */
    template<typename T>
    struct remove_cvref : Remove_cvrf_t<T> {};

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


}

#endif
