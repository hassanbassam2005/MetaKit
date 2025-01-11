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
		using type = T;
	};

}

#endif
