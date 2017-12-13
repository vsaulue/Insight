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

#ifndef NOSUCHFIELDSHELLEXCEPTION_HPP
#define	NOSUCHFIELDSHELLEXCEPTION_HPP

#include "ShellClass.hpp"
#include <stdexcept>

/**
 * Exception accessing an invalid field in a ShellClass object.
 */
class NoSuchFieldShellException : public std::invalid_argument {
public:
    /**
     * Creates a new NoSuchFieldShellException.
     *
     * @param[in] leftValue Accessed object.
     * @param[in] fieldName Name of the accessed field.
     */
    NoSuchFieldShellException(const ShellClass &leftValue, const std::string &fieldName);

    virtual const char* what() const noexcept override;

private:
    /** Message returned by what(). */
    std::string message;
};

#endif	/* NOSUCHFIELDSHELLEXCEPTION_HPP */

