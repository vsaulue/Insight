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

#ifndef PARSERFUNCTIONCALL_HPP
#define	PARSERFUNCTIONCALL_HPP

#include "ParserExpression.hpp"

#include <vector>
#include <memory>

/**
 * AST representing a function call (of the form: someMethod()).
 *
 * @todo: determine if there are only method, or if there are also global functions in the shell language.
 */
class ParserFunctionCall : public ParserExpression {
public:
    /**
     * Default constructor.
     *
     * @param functionName Name of the function or method.
     * @param arguments Sequence of arguments.
     */
    ParserFunctionCall(std::string&& functionName, std::vector<std::string>&& arguments);

    virtual ShellClass& eval(ShellInterpreter& interpreter) override;

private:
    /** Name of the function/method. */
    std::string functionName;
    /** Sequence of arguments. */
    std::vector<std::string> arguments; // @todo: handle real arguments.
};

#endif	/* PARSERFUNCTIONCALL_HPP */

