#include <iostream>
#include <assert.h>
#include <array>
#include <tuple>
#include <vector>

#include "tuple.h"
#include "type_list.h"

using namespace metakit;

namespace test
{
    /**
     * @brief Tracks the number of default constructions, copies, and moves of an object.
     */
    struct CopyStats
    {
        int n_default_constructs = 0; ///< Number of times the default constructor is called.
        int n_copies = 0;             ///< Number of copy operations performed.
        int n_moves = 0;              ///< Number of move operations performed.

        bool operator==(const CopyStats& other) const = default; ///< Compares two CopyStats objects.
    };

    /**
     * @brief Overloads the stream operator to print CopyStats.
     *
     * @param os The output stream.
     * @param stats The CopyStats object to print.
     * @return The output stream after printing.
     */
    inline std::ostream& operator<<(std::ostream& os, const CopyStats& stats)
    {
        os << "{default_constructor : " << stats.n_default_constructs
            << ", copies : " << stats.n_copies
            << ", moves : " << stats.n_moves << " }";
        return os;
    }

	template <size_t i>
	struct IndexedCopyCounter {
		struct reset_after_construct_t {};
		static constexpr reset_after_construct_t reset_after_construct{};

		inline static CopyStats stats;

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
		static CopyStats reset() {
			CopyStats old_stats = stats;
			stats.n_copies = 0;
			stats.n_moves = 0;
			stats.n_default_constructs = 0;
			return old_stats;
		}
		template <size_t i2>
		bool operator==(const IndexedCopyCounter<i2>& other) const {
			return stats == other.stats;
		}
	};
	template <size_t i>
	inline std::ostream& operator<<(std::ostream& os, const IndexedCopyCounter<i>& c) {
		os << "{ default_constructs: " << c.stats.n_default_constructs << ", copies: " << c.stats.n_copies
			<< ", moves: " << c.stats.n_moves << " }";
		return os;
	}
	namespace detail {
		static constexpr size_t default_copycounter_index = 2734987;  // just some random number so you don't easily create an
		// IndexedCopyCounter with the same index
	}  // namespace detail
	using CopyCounter = IndexedCopyCounter<detail::default_copycounter_index>;

	template <size_t index = detail::default_copycounter_index>
	IndexedCopyCounter<index> make_copy_counter() {
		return IndexedCopyCounter<index>{IndexedCopyCounter<index>::reset_after_construct};
	}

}  // namespace bits_of_q


/// This namespace contains small testing framework developed for this series to keep the number of dependencies low.
/// For any serious project, please use a well-established testing framework such as GoogleTest or Catch.
namespace bits_of_q::testing {

	struct AssertFailed : std::runtime_error {
		explicit AssertFailed(std::string_view file_path, size_t line, std::string assert_expression)
			: std::runtime_error("assert failed")
			, file(file_path)
			, line_nr(line)
			, expression(std::move(assert_expression)) {}
		std::string_view file;
		size_t line_nr;
		std::string expression;
	};

	struct AssertEqFailed : public AssertFailed {
		explicit AssertEqFailed(std::string_view file_path,
			size_t line,
			std::string assert_expression,
			std::string value_string1,
			std::string value_string2)
			: AssertFailed(file_path, line, std::move(assert_expression))
			, value1(std::move(value_string1))
			, value2(std::move(value_string2)) {}
		std::string value1;
		std::string value2;
	};

#define ASSERT(expr)                                                                                    \
	if (!(expr)) {                                                                                        \
		throw ::test::testing::AssertFailed{__FILE__, __LINE__, std::string{"ASSERT("} + #expr + ")"}; \
	}

	// Making the assumption here than the expressions can be converted to string. A more robust solution would verify if
	// this is the case using for example a requires expression instead of risking compilation errors when dealing with
	// expressions that can't be converted to string
#define ASSERT_EQ(expr1, expr2)                                                                          \
	if (!((expr1) == (expr2))) {                                                                           \
		std::stringstream ss1;                                                                               \
		std::stringstream ss2;                                                                               \
		ss1 << (expr1);                                                                                      \
		ss2 << (expr2);                                                                                      \
		throw ::bits_of_q::testing::AssertEqFailed{__FILE__,                                                 \
		                                           __LINE__,                                                 \
		                                           std::string{"ASSERT_EQ("} + #expr1 + ", " + #expr2 + ")", \
		                                           ss1.str(),                                                \
		                                           ss2.str()};                                               \
	}

	inline void output_specific_assert_info(const AssertFailed&) {}

	inline void output_specific_assert_info(const AssertEqFailed& e) {
		std::cerr << "\tvalue1: " << e.value1 << "\n\tvalue2: " << e.value2 << "\n";
	}

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

	class Tester {
		static constexpr std::string_view color_reset = "\033[0m";
		static constexpr std::string_view color_red = "\033[31m";
		static constexpr std::string_view color_green = "\033[32m";

	public:
		/// Executes the function outputting a banner at the start and end as well as printing information on exceptions
		/// thrown by the function.
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
		static void print_test_start(std::string_view test_name) {
			std::cerr << color_green << "[ RUN      ] " << color_reset << test_name << "\n";
		}
		static void print_test_end(std::string_view test_name, bool passed) {
			if (passed) {
				std::cerr << color_green << "[       OK ] " << color_reset << test_name << "\n";
			}
			else {
				std::cerr << color_red << "[  FAILED  ] " << color_reset << test_name << "\n";
			}
		}
	};

	enum class Configuration { non_const_lvalue = 0, const_lvalue, non_const_rvalue, const_rvalue };
	static constexpr std::array<std::string_view, 4> g_config_string = { "&", "const &", "&&", "const &&" };
	constexpr std::string_view config_to_string(Configuration c) {
		return g_config_string[static_cast<size_t>(c)];
	}
	template <Configuration... configs>
	class Builder;

	template <size_t n_args>
	class TesterWithBuilder {
	public:
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
		template <typename T>
		static constexpr T constexpr_pow(T num, unsigned int pow) {
			return (pow >= sizeof(unsigned int) * 8) ? 0 : pow == 0 ? 1 : num * constexpr_pow(num, pow - 1);
		}

		template <size_t config, size_t... arg_indices, typename FUNC>
		static void execute_for_config(std::index_sequence<arg_indices...>, FUNC&& function) {
			function(Builder<compute_arg_config<config, arg_indices>()...>{});
		}

		template <size_t config, size_t arg_index>
		static constexpr Configuration compute_arg_config() {
			constexpr size_t n_configurations_per_arg = 4;
			constexpr auto arg_config = static_cast<Configuration>(
				size_t(config / constexpr_pow(n_configurations_per_arg, arg_index)) % n_configurations_per_arg);
			return arg_config;
		}

		template <size_t config, size_t... arg_indices>
		static std::string get_config_str(std::index_sequence<arg_indices...>) {
			return "| " + ((std::string{ config_to_string(compute_arg_config<config, arg_indices>()) } + " | ") + ...);
		}
	};

	template <Configuration... configs>
	class Builder {
	public:
		template <typename ARG>
			requires(sizeof...(configs) == 1)
		decltype(auto) build(ARG&& arg) { return store_and_cast<configs...>(forward<ARG>(arg)); }

		template <typename... ARGS>
			requires(sizeof...(configs) > 1 && sizeof...(configs) == sizeof...(ARGS))
		decltype(auto) build(ARGS &&...args) {
			return std::tuple<decltype(store_and_cast<configs>(forward<ARGS>(args)))...>{
				store_and_cast<configs>(forward<ARGS>(args))...};
		}

	private:
		std::vector<std::unique_ptr<void, void (*)(void*)>> m_values;

		template <Configuration config, typename T>
		decltype(auto) store_and_cast(T&& t) {
			auto* copy = new std::remove_cvref_t<T>{ std::forward<T>(t) };
			void (*destructor)(void*) = [](void* ptr) { delete static_cast<remove_cvrf_t<T>*>(ptr); };
			m_values.emplace_back(copy, destructor);
			return cast_value<config>(*copy);
		}

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

} // namespace test
