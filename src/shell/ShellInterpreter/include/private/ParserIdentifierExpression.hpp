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

#ifndef PARSERIDENTIFIEREXPRESSION_HPP
#define	PARSERIDENTIFIEREXPRESSION_HPP

#include "ParserExpression.hpp"
#include "ShellClass.hpp"

/**
 * AST representing a single identifier (ex: someField | someVar).
 */
class ParserIdentifierExpression : public ParserExpression {
public:
    /**
     * Default constructor.
     * @param name Name of the identifier.
     */
    ParserIdentifierExpression(std::string&& name);

    virtual ShellClass& eval(ShellInterpreter& interpreter) override;

private:
    /** Name of the identifier. */
    std::string identifier;
};

#endif	/* PARSERIDENTIFIEREXPRESSION_HPP */

