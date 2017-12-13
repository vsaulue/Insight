/* This file is part of Insight.
 * Copyright (C) 2017 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#ifndef X3EXT_HPP
#define	X3EXT_HPP

#include <boost/spirit/home/x3.hpp>

namespace boost { namespace spirit { namespace x3 { namespace ext {

    /**
     * Parser injecting a local variable into a given sub-parser.
     */
    template <typename Subject, typename ID, typename T>
    struct local_directive
    : with_value_holder<Subject, local_directive<Subject, ID, T>, T> {
        /** Parent type of this class. */
        typedef with_value_holder<Subject, local_directive<Subject, ID, T>, T> base_type;

        /** Spirit internals ? */
        static bool const is_pass_through_unary = true;
        /** Spirit internals ? */
        static bool const handles_container = Subject::handles_container;
        /** Spirit internals ? */
        typedef Subject subject_type;

        /**
         * Default constructor.
         *
         * @param[in] subject sub-parser that will have the local variable in its context.
         * @param[in] val initial value of the local variable.
         */
        local_directive(Subject const& subject, T const& val)
        : base_type(subject, val) {
        }

        /** Run the parser on the given input.
         *
         * See @p x3::parse of @p x3::phrase_parse .
         *
         * @note This is clearly a compile-time polymorphic method. I don't know where it should be
         * documented, but likely not here.
         */
        template <typename Iterator, typename Context
        , typename RContext, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
                , Context const& context, RContext& rcontext, Attribute& attr) const {
            T localValue(this->val);
            return this->subject.parse(
                    first, last
                    , make_context<ID>(localValue, context)
                    , rcontext
                    , attr);
        }
    };

    /**
     * Transitional object used to generate a parser with a local variable.
     *
     * @tparam ID (dummy class) used to get/retrieve the associated value from the context.
     * @tparam T type of the stored variable.
     */
    template <typename ID, typename T>
    struct local_gen {
        /** Reference to the initial value of the variable. */
        const T& val;

        /**
         * Default constructor.
         *
         * @param[in] val Initial value of the variable.
         */
        local_gen(const T& val) : val(val) {
        }

        /**
         * Creates a Spirit parser from a given sub-parser, adding a local variable.
         *
         * This parser is the same as the sub-parser, adding the initialization of
         * a new object of type T and its injection in the context of the sub-parser.
         *
         * @tparam Subject Type of the sub-parser.
         * @param subject sub-parser.
         */
        template <typename Subject>
        local_directive<typename extension::as_parser<Subject>::value_type, ID, T>
        operator[](const Subject& subject) const {
            return { as_parser(subject), val };
        }
    };

    /**
     * Creates a local variable that can be accessed through the context in the Subject.
     *
     * The type T must have a copy constructor.
     *
     * Similar to the @p x3::with(var)[subParser] directive, but @p with will just store a reference to var inside
     * the context used by subParser. @p x3::ext::local(var)[subParser] will create a brand new object of type
     * T accessible from the context of subParser, initialized to the value of @p var .
     *
     * @tparam ID tag (dummy class) used to get/retrieve the associated value from the context.
     * @tparam T type of the stored variable.
     * @param[in] val Initial value of the variable.
     * @return An object that can be used to generate a parser (with its operator @p [] ).
     */
    template <typename ID, typename T>
    inline local_gen<ID, T> local(const T& val) {
        return { val };
    }
    
}}}}

#endif	/* X3EXT_HPP */

