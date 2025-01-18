#include <iostream>
#include <list>

#include "tuple.h"
#include "type_list.h"

using namespace metakit;

namespace test
{
	struct CopyStats
	{
		int n_defaul_Constructor = 0;
		int n_copies = 0;
		int n_moves = 0;

		bool operator==(const CopyStats& other) const = default;
	};

	inline std::ostream& operator<<(std::ostream& os, const CopyStats& stats)
	{
		os << "{default_constructor :" << stats.n_defaul_Constructor << ", copies" << stats.n_copies << ", moves" << stats.n_moves << "}";
		return os;
	}

	struct CopyCounter
	{
		inline static CopyStats stats;

		CopyCounter() { stats.n_defaul_Constructor++; }
		CopyCounter(const CopyStats&) { stats.n_copies++; }
		CopyCounter& operator=(const CopyCounter&)
		{
			stats.n_copies++;
			return *this;
		}
		CopyCounter(CopyCounter&&) { stats.n_moves++; }
		CopyCounter& operator=(CopyCounter&&)
		{
			stats.n_moves++;
			return *this;
		}

		static CopyCounter rest()
		{
			CopyStats old_stats = stats;
			stats.n_copies = 0;
			stats.n_defaul_Constructor = 0;
			stats.n_moves = 0;
			return old_stats;
		}

	};
}

