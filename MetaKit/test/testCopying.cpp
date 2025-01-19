#include <iostream>
#include <assert.h>

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
        int n_defaul_Constructor = 0; ///< Number of times the default constructor is called.
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
        os << "{default_constructor : " << stats.n_defaul_Constructor
            << ", copies : " << stats.n_copies
            << ", moves : " << stats.n_moves << " }";
        return os;
    }

    /**
     * @brief A utility class to count the number of default constructions, copies, and moves.
     */
    struct CopyCounter
    {
        inline static CopyStats stats; ///< Stores the statistics for copy/move operations.

        /**
         * @brief Default constructor increments the default constructor count.
         */
        CopyCounter() { stats.n_defaul_Constructor++; }

        /**
         * @brief Copy constructor increments the copy count.
         */
        CopyCounter(const CopyStats&) { stats.n_copies++; }

        /**
         * @brief Copy assignment operator increments the copy count.
         *
         * @return Reference to the assigned object.
         */
        CopyCounter& operator=(const CopyCounter&)
        {
            stats.n_copies++;
            return *this;
        }

        /**
         * @brief Move constructor increments the move count.
         */
        CopyCounter(CopyCounter&&) { stats.n_moves++; }

        /**
         * @brief Move assignment operator increments the move count.
         *
         * @return Reference to the assigned object.
         */
        CopyCounter& operator=(CopyCounter&&)
        {
            stats.n_moves++;
            return *this;
        }

        /**
         * @brief Resets the copy statistics and returns the previous statistics.
         *
         * @return The old CopyStats before reset.
         */
        static CopyCounter reset()
        {
            CopyStats old_stats = stats;
            stats.n_copies = 0;
            stats.n_defaul_Constructor = 0;
            stats.n_moves = 0;
            return old_stats;
        }
    };

} // namespace test
