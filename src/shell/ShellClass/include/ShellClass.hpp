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

#ifndef SHELLTYPE_HPP
#define	SHELLTYPE_HPP

#include <sstream>
#include <unordered_map>

/**
 * Abstract class of an object that can be manipulated throught the shell.
 */
class ShellClass {
public:
    /**
     * Get the class name that will be displayed by the shell for this object.
     *
     * @return A name identifying the type of this object.
     */
    virtual const std::string& getShellClassName() const;

    /**
     * Print the type and content of this object on the given stream.
     *
     * @param stream Output stream.
     */
    virtual void shellPrint(std::ostream& stream) const;

    /**
     * Get a field by name in this object.
     *
     * Shell fields doesn't have to correspond to a C++ fields: the C++ class can have fields that the shell class
     * doesn't have, and vice versa.
     *
     * @param[in] fieldName Name of the field.
     * @return The object associated to this name.
     * @throws NoSuchFieldShellException if this object doesn't have a field of the given name.
     */
    virtual ShellClass& getField(const std::string& fieldName);

    /**
     * Evaluate a mathod by name in this object.
     *
     * Shell methods doesn't have to correspond to a C++ methods: the C++ class can have methods that the shell class
     * doesn't have, and vice versa.
     *
     * @param[in] methodName Name of the method.
     * @throws NoSuchMethodShellException if this object doesn't have a method of the given name.
     */
    virtual void evalMethod(const std::string& methodName);
};

#endif	/* SHELLTYPE_HPP */

