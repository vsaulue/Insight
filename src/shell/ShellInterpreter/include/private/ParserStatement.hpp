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

#ifndef PARSERSTATEMENT_HPP
#define	PARSERSTATEMENT_HPP

#include "ShellClass.hpp"
#include "ShellInterpreter.hpp"

/**
 * AST node of a parsed statement.
 *
 * A statement is (currently) just an expression followed by ';'.
 */
class ParserStatement {
public:
    /**
     * Evaluate the statement in the context of a shell.
     *
     * @param[in,out] interpreter Interpreter in which the statement is evaluated.
     * @return An object holding the result of the evaluation.
     */
    virtual ShellClass& eval(ShellInterpreter& interpreter) = 0;
};

#endif	/* PARSERSTATEMENT_HPP */

