/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tuple utilities
 */

#pragma once

#include <tuple>

namespace kF::Core
{
    namespace Internal
    {
        /** @brief Helper used to recursivly search for index of a type */
        template<typename Search, typename Tuple>
        struct SearchTupleElementIndexHelper;

        /** @brief Helper reached end of tuple */
        template<typename Search>
        struct SearchTupleElementIndexHelper<Search, std::tuple<>>
        {
            static constexpr std::size_t Value = 0;
        };

        /** @brief Helper found index and has to ensure 'Search' appears only once */
        template<typename Search, typename... Nexts>
        struct SearchTupleElementIndexHelper<Search, std::tuple<Search, Nexts...>>
        {
            static constexpr std::size_t Value = 0;

            using NextsTuple = std::tuple<Nexts...>;

            static_assert(SearchTupleElementIndexHelper<Search, NextsTuple>::Value == std::tuple_size_v<NextsTuple>,
                "TupleElementIndex: 'Search' type appears more than once in 'Tuple'");
        };

        /** @brief Helper did not found 'Search' type and continue searching */
        template<typename Search, typename First, typename... Nexts>
        struct SearchTupleElementIndexHelper<Search, std::tuple<First, Nexts...>>
        {
            using RestTuple = std::tuple<Nexts...>;

            static constexpr std::size_t Value = 1 + SearchTupleElementIndexHelper<Search, RestTuple>::Value;
        };

        /** @brief Start helper recursion and ensure the index is in tuple range */
        template<typename Search, typename Tuple>
        struct SearchTupleElementIndex
        {
            static constexpr std::size_t Value = SearchTupleElementIndexHelper<Search, Tuple>::Value;

            static_assert(Value < std::tuple_size_v<Tuple>, "TupleElementIndex: 'Search' type does not appear in 'Tuple'");
        };
    }

    /** @brief Get the index of a type inside a tuple */
    template<typename Search, typename Tuple>
    constexpr std::size_t TupleElementIndex = Internal::SearchTupleElementIndex<Search, Tuple>::Value;

    /** @brief Check if a type match a tuple element */
    template<typename Search, typename Tuple>
    constexpr bool TupleContainsElement = []<typename ...Types>([[maybe_unused]] const std::tuple<Types...> *) {
        return (std::is_same_v<Search, Types> || ...);
    }(static_cast<const Tuple *>(nullptr));
}