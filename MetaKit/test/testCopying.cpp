#include <array>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>
#include <utility>
#include <vector>
#include <vector>

#include "tuple.h"
#include "type_list.h"
#include "helper_.h"

using namespace metakit;

namespace test
{
	/* @brief Tracks the number of default constructions, copies, and moves of an object. */
	struct CopyStats
	{
		int n_default_constructs = 0; ///< Number of times the default constructor is called.
		int n_copies = 0;             ///< Number of copy operations performed.
		int n_moves = 0;              ///< Number of move operations performed.

		/* @brief Compares two CopyStats objects for equality.
		   @return true if both objects have the same statistics, false otherwise. */
		bool operator==(const CopyStats& other) const = default;
	};

	/* @brief Overloads the stream operator to print CopyStats.
	   @param os The output stream to write to.
	   @param stats The CopyStats object to print.
	   @return The output stream after printing. */
	inline std::ostream& operator<<(std::ostream& os, const CopyStats& stats)
	{
		os << "{default_constructor : " << stats.n_default_constructs
			<< ", copies : " << stats.n_copies
			<< ", moves : " << stats.n_moves << " }";
		return os;
	}

	/* @brief Template struct to count copy and move operations for indexed types. */
	template <size_t i>
	struct IndexedCopyCounter {
		struct reset_after_construct_t {};
		static constexpr reset_after_construct_t reset_after_construct{};

		inline static CopyStats stats; ///< Static variable to hold copy statistics.

		/* @brief Default constructor increments the default construction count. */
		explicit IndexedCopyCounter(reset_after_construct_t) { reset(); }
		IndexedCopyCounter() { stats.n_default_constructs++; }
		IndexedCopyCounter(const IndexedCopyCounter&) { stats.n_copies++; }
		IndexedCopyCounter& operator=(const IndexedCopyCounter&) {
			stats.n_copies++;
			return *this;
		}
		IndexedCopyCounter(IndexedCopyCounter&&) noexcept { stats.n_moves++; };
		IndexedCopyCounter& operator=(IndexedCopyCounter&&) noexcept {
			stats.n_moves++;
			return *this;
		}
		/* @brief Resets the copy statistics and returns the old stats.
		   @return The previous CopyStats before reset. */
		static CopyStats reset() {
			CopyStats old_stats = stats;
			stats.n_copies = 0;
			stats.n_moves = 0;
			stats.n_default_constructs = 0;
			return old_stats;
		}
		/* @brief Compares two IndexedCopyCounter objects based on their stats.
		   @param other The other IndexedCopyCounter to compare against.
		   @return true if both objects have the same statistics, false otherwise. */
		template <size_t i2>
		bool operator==(const IndexedCopyCounter<i2>& other) const {
			return stats == other.stats;
		}
	};

	/* @brief Overloads the stream operator to print IndexedCopyCounter statistics.
	   @param os The output stream to write to.
	   @param c The IndexedCopyCounter object to print.
	   @return The output stream after printing. */
	template <size_t i>
	inline std::ostream& operator<<(std::ostream& os, const IndexedCopyCounter<i>& c) {
		os << "{ default_constructs: " << c.stats.n_default_constructs << ", copies: " << c.stats.n_copies
			<< ", moves: " << c.stats.n_moves << " }";
		return os;
	}

	namespace detail {
		static constexpr size_t default_copycounter_index = 68474;  // Random number to avoid index collisions.
	}  // namespace detail

	/* @brief Alias for the default IndexedCopyCounter. */
	using CopyCounter = IndexedCopyCounter<detail::default_copycounter_index>;

	/* @brief Creates a new IndexedCopyCounter with reset statistics.
	   @return A new IndexedCopyCounter with reset statistics. */
	template <size_t index = detail::default_copycounter_index>
	IndexedCopyCounter<index> make_copy_counter() {
		return IndexedCopyCounter<index>{IndexedCopyCounter<index>::reset_after_construct};
	}	

}  // namespace test

/* @brief This namespace contains a small testing framework to keep dependencies low. */
namespace test::testing {

	/* @brief Exception thrown when an assertion fails. */
	struct AssertFailed : std::runtime_error {
		/* @brief Constructs an AssertFailed exception.
		   @param file_path The file where the assertion failed.
		   @param line The line number where the assertion failed.
		   @param assert_expression The expression that failed. */
		explicit AssertFailed(std::string_view file_path, size_t line, std::string assert_expression)
			: std::runtime_error("assert failed")
			, file(file_path)
			, line_nr(line)
			, expression(metakit::move(assert_expression)) {}
		std::string_view file; ///< File where the assertion failed.
		size_t line_nr;        ///< Line number where the assertion failed.
		std::string expression; ///< The assertion expression that failed.
	};

	/* @brief Exception thrown when an equality assertion fails. */
	struct AssertEqFailed : public AssertFailed {
		/* @brief Constructs an AssertEqFailed exception.
		   @param file_path The file where the assertion failed.
		   @param line The line number where the assertion failed.
		   @param assert_expression The expression that failed.
		   @param value_string1 The string representation of the first value.
		   @param value_string2 The string representation of the second value. */
		explicit AssertEqFailed(std::string_view file_path,
			size_t line,
			std::string assert_expression,
			std::string value_string1,
			std::string value_string2)
			: AssertFailed(file_path, line, metakit::move(assert_expression))
			, value1(metakit::move(value_string1))
			, value2(metakit::move(value_string2)) {}
		std::string value1; ///< The string representation of the first value.
		std::string value2; ///< The string representation of the second value.
	};

	/* @brief Macro to assert a condition.
	   @param expr The expression to evaluate. */
#define ASSERT(expr)                                                                                    \
	if (!(expr)) {                                                                                        \
		throw test::testing::AssertEqFailed{__FILE__, __LINE__, std::string{"ASSERT("} + #expr + ")"}; \
	}

	   /* @brief Macro to assert that two values are equal.
		  @param expr1 The first expression to compare.
		  @param expr2 The second expression to compare. */
#define ASSERT_EQ(expr1, expr2)                                                                          \
	if (!((expr1) == (expr2))) {                                                                           \
		std::stringstream ss1;                                                                               \
		std::stringstream ss2;                                                                               \
		ss1 << (expr1);                                                                                      \
		ss2 << (expr2);                                                                                      \
		throw test::testing::AssertEqFailed{__FILE__,                                                 \
		                                          size_t(__LINE__),                                                 \
		                                           std::string{"ASSERT_EQ("} + #expr1 + ", " + #expr2 + ")", \
		                                           ss1.str(),                                                \
		                                           ss2.str()};                                               \
	}

		  /* @brief Outputs specific information about an AssertFailed exception.
			 @param e The AssertFailed exception to output information for. */
	inline void output_specific_assert_info(const AssertFailed&) {}

	/* @brief Outputs specific information about an AssertEqFailed exception.
	   @param e The AssertEqFailed exception to output information for. */
	inline void output_specific_assert_info(const AssertEqFailed& e) {
		std::cerr << "\tvalue1: " << e.value1 << "\n\tvalue2: " << e.value2 << "\n";
	}

	/* @brief Outputs information about the last thrown exception. */
	static void output_exception_info() {
		auto assert_output = [](const auto& e) {
			std::cerr << "\nException thrown at " << e.file << ":" << e.line_nr << " :\n";
			std::cerr << e.expression << " evaluated to false\n";
			output_specific_assert_info(e);
			};
		try {
			throw;
		}
		catch (const AssertEqFailed& e) {
			assert_output(e);
		}
		catch (const AssertFailed& e) {
			assert_output(e);
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
		catch (...) {
			std::cerr << "Unknown exception thrown\n";
		}
	}

	/* @brief Class to manage and execute tests with exception handling and output. */
	class Tester {
		static constexpr std::string_view color_reset = "\033[0m";
		static constexpr std::string_view color_red = "\033[31m";
		static constexpr std::string_view color_green = "\033[32m";

	public:
		/* @brief Executes a test function and handles exceptions.
		   @param test_name The name of the test to display.
		   @param function The test function to execute. */
		template <typename FUNC>
		static void test(std::string_view test_name, FUNC&& function) {
			print_test_start(test_name);
			try {
				function();
			}
			catch (...) {
				output_exception_info();
				print_test_end(test_name, false);
				return;
			}
			print_test_end(test_name, true);
		}

	private:
		/* @brief Prints the start of a test.
		   @param test_name The name of the test being run. */
		static void print_test_start(std::string_view test_name) {
			std::cerr << color_green << "[  RUN  ] " << color_reset << test_name << "\n";
		}
		/* @brief Prints the end of a test.
		   @param test_name The name of the test that has finished.
		   @param passed Indicates whether the test passed or failed. */
		static void print_test_end(std::string_view test_name, bool passed) {
			if (passed) {
				std::cerr << color_green << "[  OK  ] " << color_reset << test_name << "\n";
			}
			else {
				std::cerr << color_red << "[  FAILED  ] " << color_reset << test_name << "\n";
			}
		}
	};

	/* @brief Enumeration for different configurations of value references. */
	enum class Configuration { non_const_lvalue = 0, const_lvalue, non_const_rvalue, const_rvalue };
	static constexpr std::array<std::string_view, 4> g_config_string = { "&", "const &", "&&", "const &&" };

	/* @brief Converts a Configuration enum to its string representation.
	   @param c The Configuration enum to convert.
	   @return The string representation of the configuration. */
	constexpr std::string_view config_to_string(Configuration c) {
		return g_config_string[static_cast<size_t>(c)];
	}

	/* @brief Template class for building test configurations. */
	template <Configuration... configs>
	class Builder;

	/* @brief Template class for testing with multiple configurations. */
	template <size_t n_args>
	class TesterWithBuilder {
	public:
		/* @brief Executes a test function for all configurations.
		   @param test_name The name of the test to display.
		   @param function The test function to execute. */
		template <typename FUNC>
		static void test(std::string_view test_name, FUNC&& function) {
			auto test_func = [&]() {
				static constexpr size_t n_configurations = constexpr_pow(4U, n_args);
				int failed_configs = 0;
				static_for<0, n_configurations>([&](auto i) {
					try {
						execute_for_config<i.value>(make_index_sequence<n_args>{}, function);
					}
					catch (...) {
						output_exception_info();
						std::cerr << "Using builder config: " << get_config_str<i.value>(make_index_sequence<n_args>{}) << "\n";
						failed_configs++;
					}
					});
				if (failed_configs > 0) {
					throw std::runtime_error("Test failed for " + std::to_string(failed_configs) + " configurations");
				};
				};
			Tester::test(test_name, std::move(test_func));
		}

	private:
		/* @brief Computes the power of a number at compile time.
		   @param num The base number.
		   @param pow The exponent.
		   @return The result of num raised to the power of pow. */
		template <typename T>
		static constexpr T constexpr_pow(T num, unsigned int pow) {
			return (pow >= sizeof(unsigned int) * 8) ? 0 : pow == 0 ? 1 : num * constexpr_pow(num, pow - 1);
		}

		/* @brief Executes a test function for a specific configuration.
		   @param config The configuration index.
		   @param arg_indices The indices of the arguments.
		   @param function The test function to execute. */
		template <size_t config, size_t... arg_indices, typename FUNC>
		static void execute_for_config(index_sequence<arg_indices...>, FUNC&& function) {
			function(Builder<compute_arg_config<config, arg_indices>()...>{});
		}

		/* @brief Computes the configuration for a specific argument index.
		   @param config The configuration index.
		   @param arg_index The index of the argument.
		   @return The Configuration enum for the argument. */
		template <size_t config, size_t arg_index>
		static constexpr Configuration compute_arg_config() {
			constexpr size_t n_configurations_per_arg = 4;
			constexpr auto arg_config = static_cast<Configuration>(
				size_t(config / constexpr_pow(n_configurations_per_arg, arg_index)) % n_configurations_per_arg);
			return arg_config;
		}

		/* @brief Gets the string representation of a configuration.
		   @param config The configuration index.
		   @param arg_indices The indices of the arguments.
		   @return The string representation of the configuration. */
		template <size_t config, size_t... arg_indices>
		static std::string get_config_str(index_sequence<arg_indices...>) {
			return "| " + ((std::string{ config_to_string(compute_arg_config<config, arg_indices>()) } + " | ") + ...);
		}
	};

	/* @brief Template class for building test configurations. */
	template <Configuration... configs>
	class Builder {
	public:
		/* @brief Builds a single value based on the configuration.
		   @param arg The argument to build.
		   @return The constructed value. */
		template <typename ARG>
			requires(sizeof...(configs) == 1)
		decltype(auto) build(ARG&& arg) { return store_and_cast<configs...>(forward<ARG>(arg)); }

		/* @brief Builds a tuple of values based on the configurations.
		   @param args The arguments to build.
		   @return A tuple containing the constructed values. */
		template <typename... ARGS>
			requires(sizeof...(configs) > 1 && sizeof...(configs) == sizeof...(ARGS))
		decltype(auto) build(ARGS &&...args) {
			return std::tuple<decltype(store_and_cast<configs>(forward<ARGS>(args)))...>{
				store_and_cast<configs>(forward<ARGS>(args))...};
		}

	private:
		std::vector<std::unique_ptr<void, void (*)(void*)>> m_values; ///< Vector to hold constructed values.

		/* @brief Stores and casts a value based on the configuration.
		   @param t The value to store.
		   @return The casted value. */
		template <Configuration config, typename T>
		decltype(auto) store_and_cast(T&& t) {
			auto* copy = new std::remove_cvref_t<T>{ forward<T>(t) };
			void (*destructor)(void*) = [](void* ptr) { delete static_cast<remove_cvrf_t<T>*>(ptr); };
			m_values.emplace_back(copy, destructor);
			return cast_value<config>(*copy);
		}

		/* @brief Casts a value based on the configuration.
		   @param value The value to cast.
		   @return The casted value. */
		template <Configuration config, typename T>
		decltype(auto) cast_value(T& value) {
			if constexpr (config == Configuration::non_const_lvalue) {
				return static_cast<remove_cv_t<T> &>(value);
			}
			if constexpr (config == Configuration::const_lvalue) {
				return static_cast<const remove_cv_t<T> &>(value);
			}
			if constexpr (config == Configuration::non_const_rvalue) {
				return static_cast<remove_cv_t<T>&&>(value);
			}
			if constexpr (config == Configuration::const_rvalue) {
				return static_cast<const remove_cv_t<T>&&>(value);
			}
		}
	};

} // namespace Test::testing