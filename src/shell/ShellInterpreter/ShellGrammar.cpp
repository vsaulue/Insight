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

#include "ShellGrammar.hpp"

#include <boost/spirit/home/x3.hpp>
#include "x3Ext.hpp"

#include "ParserStatement.hpp"
#include "ParserExpression.hpp"
#include "ParserIdentifierExpression.hpp"
#include "ParserFieldSelection.hpp"
#include "ParserFunctionCall.hpp"

namespace x3 = boost::spirit::x3;

namespace ShellGrammar {
    using x3::rule;
    using x3::lit;
    using x3::lexeme;
    using x3::eps;
    using x3::char_;
    using x3::_val;
    using x3::_attr;
    using std::move;
    using std::make_shared;

    using x3::ext::local;

    rule<class x3Identifier, std::string> const identifier = "identifier";
    rule<class x3Arguments, std::vector<std::string>> const arguments = "arguments";
    rule<class x3Primary, std::shared_ptr<ParserExpression>> const primary = "primary";
    rule<class x3Expression, std::shared_ptr<ParserExpression>> const expression = "expression";
    rule<class x3Statement, std::shared_ptr<ParserStatement>> const statement = "statement";

    namespace arg {
        auto initVector = [](auto& ctx) {
            _val(ctx) = std::vector<std::string>();
        };
    }

    namespace pri {

        struct id {};

        auto init = [](auto& ctx) {
            std::string& identifier = x3::get<id>(ctx);
            identifier = move(_attr(ctx));
        };

        auto makeIdentifier = [](auto &ctx) {
            std::string& identifier = x3::get<id>(ctx);
            _val(ctx) = make_shared<ParserIdentifierExpression>(move(identifier));
        };

        auto makeFunctionCall = [](auto &ctx) {
            std::string& identifier = x3::get<id>(ctx);
            _val(ctx) = make_shared<ParserFunctionCall>(move(identifier), move(_attr(ctx)));
        };
    }

    namespace exp {

        struct id {};

        auto newPrimary = [](auto& ctx) {
            _val(ctx) = move(_attr(ctx));
        };

        auto setId = [](auto& ctx) {
            x3::get<id>(ctx) = move(_attr(ctx));
        };

        auto newFieldSelection = [](auto& ctx) {
            _val(ctx) = make_shared<ParserFieldSelection>(move(_val(ctx)), move(x3::get<id>(ctx)));
        };
    }

    auto const identifier_def = lexeme[char_("a-zA-Z_") >> *char_("a-zA-Z0-9_")];
    auto const arguments_def = lit('(')[arg::initVector] >> lit(')'); // TODO: accept arguments.
    auto const primary_def = local<pri::id>(std::string())[ identifier[pri::init] >> (arguments[pri::makeFunctionCall] | eps[pri::makeIdentifier])];
    auto const expression_def = primary[exp::newPrimary] >> *local<exp::id>(std::string())[lit('.') >> identifier[exp::setId] >> eps[exp::newFieldSelection]]; // TODO handle method call
    auto const statement_def = expression >> lit(';');

    BOOST_SPIRIT_DEFINE(identifier, arguments, primary, expression, statement);
}

bool parse(std::string::const_iterator& first, std::string::const_iterator& last, std::shared_ptr<ParserStatement>& statement) {
    return x3::phrase_parse(first, last, ShellGrammar::statement, x3::blank, statement);
}