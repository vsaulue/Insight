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

#ifndef SHELLGRAMMAR_HPP
#define	SHELLGRAMMAR_HPP

#include <memory>

#include "ParserStatement.hpp"

/**
 * Parse a string using the shell grammar.
 *
 * @param[in,out] first Iterator to the beginning of the string (will be placed after the last parsed character).
 * @param[in] last Iterator to the end of the string.
 * @param[out] statement Constructed AST
 * @return true if and only if the beginning of the string was matched.
 */
bool parse(std::string::const_iterator& first, std::string::const_iterator& last, std::shared_ptr<ParserStatement>& statement);

#endif	/* SHELLGRAMMAR_HPP */

