/*
 * This file is part of Insight.
 * Copyright (C) 2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
 *
 * Insight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Insight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Insight.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LISTWITHEXP_HPP
#define LISTWITHEXP_HPP

#include "constexpr_types.hpp"

namespace meta {
    /**
     * Item of a ListWithExp list.
     */
    template<typename Id, int e=1>
    struct ListWithExpItem {
    public:
        // Items with a 0 exponent should simply be removed from the list.
        static_assert(e!=0, "Invalid exponent.");
        /** Tag type used as primary key in the list. */
        using Identifier = Id;
        /** Value of the exponent associated to Indentifier. */
        static constexpr int exp = e;
    };

    /**
     * Generic compile-time list of types, associated to an integer exponent.
     *
     * This list contains ListWithExpItem objects. The key of each list item is
     * unique in a given list.
     */
    template<typename... BaseDims>
    struct ListWithExp;

    // Specialisation for an empty list.
    template<>
    struct ListWithExp<> {
        /** Number of items in this list. */
        static constexpr std::size_t length = 0;

        /**
         * Gets the exponent associated to the given indentifier in this list.
         * @return The exponent associated to the given identifier.
         * @tparam Identifier Item to look up in this list.
         */
        template<typename Identifier>
        static constexpr int getExp() {
            return 0;
        }

        /**
         * Tests if this list contains the same types & exponents as another list.
         * @return True if the lists have the same values.
         * @tparam OtherList Other list compared to this type.
         */
        template<typename OtherList>
        static constexpr bool is_same() {
            return (OtherList::length == length);
        }

        /**
         * Add the specified value to the item with the given Identifier.
         *
         * Creates a new item if no items with Identifier exists. Otherwise it Modifies
         * or deletes the existing item.
         *
         * @tparam Identifier Identifier to modify in the list.
         * @tparam exp Value to add to the item associated with identifier.
         */
        template<typename Identifier, int exp>
        static constexpr auto addExp(constexpr_int<exp> = {}) {
            return ListWithExp<ListWithExpItem<Identifier,exp>>();
        }

        /**
         * Tests if this list is included in another list.
         * @return True if the other list contains this one.
         * @tparam OtherList List to compare this type to.
         */
        template<typename OtherList>
        static constexpr bool isIncludedIn() {
            return true;
        }

        /**
         * Multiply all the exponents in the list by the given value.
         *
         * @tparam i Value to multiply to the items in the list.
         */
        template<int i>
        constexpr auto operator[](constexpr_int<i>) const {
            return ListWithExp{};
        }
    };

    namespace details {
        /** Builds a new list, by inserting a new item at the head of the list.*/
        template<typename Identifier, int exp, typename... OtherList>
        static constexpr auto addHead(ListWithExp<OtherList...>) {
            return ListWithExp<ListWithExpItem<Identifier,exp>, OtherList...>();
        }
    }

    template<typename Head, typename... Tail>
    struct ListWithExp<Head, Tail...> {
        /** List containg the Tail... items.*/
        using TailList = ListWithExp<Tail...>;
        /** Identifier of the head of this type. */
        using HeadId = typename Head::Identifier;
        /** Number of items in this list. */
        static constexpr std::size_t length = 1 + TailList::length;

        // See ListWithExp<>::template getExp();
        template<typename Identifier>
        static constexpr int getExp() {
            if constexpr (std::is_same<Identifier,HeadId>::value) {
                return Head::exp;
            } else {
                return TailList::template getExp<Identifier>();
            }
        }

        // See ListWithExp<>::template addExp();
        template<typename Identifier, int i>
        static constexpr auto addExp(constexpr_int<i> val={}) {
            if constexpr (std::is_same<Identifier,HeadId>::value) {
                if constexpr (i + Head::exp == 0) {
                    return TailList();
                } else {
                    return details::addHead<HeadId,i+Head::exp>(TailList());
                }
            } else {
                return details::addHead<HeadId,Head::exp>(TailList().template addExp<Identifier>(val));
            }
        }

        // See ListWithExp<>::template isIncludedIn();
        template<typename OtherList>
        static constexpr bool isIncludedIn() {
            return (OtherList::template getExp<HeadId>() == Head::exp) && TailList::template isIncludedIn<OtherList>();
        }

        // See ListWithExp<>::template isIncludedIn();
        template<typename OtherDim>
        static constexpr bool is_same() {
            return (length == OtherDim::length) && isIncludedIn<OtherDim>();
        }

        // See ListWithExp<>::operator[];
        template<int i>
        constexpr auto operator[](constexpr_int<i> val) const {
            if constexpr (i == 0) {
                return ListWithExp<>{};
            } else {
                return details::addHead<HeadId,Head::exp*i>(TailList{}[val]);
            }
        }

        // Uniqueness constraint of identifiers in the list.
        static_assert(0 == TailList::template getExp<HeadId>(),"Duplicated identifiers are not allowed.");
    };

    /**
     * Tests if two ListWithExp are equals.
     *
     * Two lists are equals if they contains exactly the same identifiers, and their
     * associated exponents are equal. The order of the identifier in the lists does not
     * matter.
     */
    template<typename... Items1, typename... Items2>
    constexpr bool operator==(ListWithExp<Items1...> l1,ListWithExp<Items2...> l2) {
        return decltype(l1)::template is_same<decltype(l2)>();
    }

    /**
     * Tests if two ListWithExp are differents.
     *
     * See operator==.
     */
    template<typename... Items1, typename... Items2>
    constexpr bool operator!=(ListWithExp<Items1...> l1,ListWithExp<Items2...> l2) {
        return !(l1 == l2);
    }

    namespace details {
        /**
         * Implementation of the multiplication of ListWithExp objects.
         *
         * @tparam List1 Type of the left operand.
         * @tparam List2 Type of the right operand.
         */
        template<typename List1, typename List2>
        struct mult;

        // Specialisation when List1 is empty.
        template<typename List2>
        struct mult<ListWithExp<>, List2> {
            using value_type = List2;
            static constexpr value_type value{};
        };

        // Specialisation when List1 is not empty.
        template<typename Id1, int exp1, typename... Tail1, typename List2>
        struct mult<ListWithExp<ListWithExpItem<Id1,exp1>,Tail1...>, List2> {
            using value_type = decltype((ListWithExp<Tail1...>() * List2()).template addExp<Id1,exp1>());
            static constexpr value_type value{};
        };
    }

    /**
     * Multiply two ListWithExp types.
     *
     * Computes a list by merging the all the identifiers of each list. If an
     * identifier is present in both lists, the exponent of in the result list is
     * the sum of the exponents from the inputs.
     *
     * @tparam ListWithExpItem elements of the left operand.
     * @tparam ListWithExpItem elements of the right operand.
     */
    template<typename... List1, typename... List2>
    constexpr auto operator*(ListWithExp<List1...>, ListWithExp<List2...>) {
        return details::mult<ListWithExp<List1...>, ListWithExp<List2...>>::value;
    }

    /**
     * Divides two ListWithExp types.
     *
     * Computes a list by merging the all the identifiers of each list. If an
     * identifier is present in both lists, the exponent of in the result list is
     * the difference of the exponents from the inputs.
     *
     * @tparam ListWithExpItem elements of the left operand.
     * @tparam ListWithExpItem elements of the right operand.
     */
    template<typename... List1, typename... List2>
    constexpr auto operator/(ListWithExp<List1...> d1, ListWithExp<List2...> d2) {
        return d1 * d2[-constexpr_int<1>()];
    }
}

#endif /* LISTWITHEXP_HPP */
