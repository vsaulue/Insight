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

#ifndef PARSERFIELDSELECTION_HPP
#define	PARSERFIELDSELECTION_HPP

#include "ParserExpression.hpp"
#include "ShellClass.hpp"

#include <memory>

/**
 * AST for a select expression (of the form: myObject.someField).
 */
class ParserFieldSelection : public ParserExpression {
public:
    /**
     * Default constructor.
     *
     * @param leftOperand AST representing the left operand of the selection.
     * @param fieldName Name of the selected filed.
     */
    ParserFieldSelection(std::shared_ptr<ParserExpression>&& leftOperand, std::string&& fieldName);

    virtual ShellClass& eval(ShellInterpreter& interpreter) override;

private:
    /** Left operand of the selection. */
    std::shared_ptr<ParserExpression> leftOperand;
    /** Name of the selected field. */
    std::string fieldName;
};

#endif	/* PARSERFIELDSELECTION_HPP */

