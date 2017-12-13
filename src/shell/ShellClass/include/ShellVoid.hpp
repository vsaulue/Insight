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

#ifndef SHELLVOID_HPP
#define	SHELLVOID_HPP

#include "ShellClass.hpp"

/**
 * Singleton object corresponding to void (object that represents nothing).
 */
class ShellVoid : public ShellClass {
protected:
    /**
     * Default constructor.
     *
     * Protected to ensure that this class is a singleton.
     */
    ShellVoid();
public:
    /**
     * Get the unique instance of ShellVoid of this program.
     *
     * @return The unique instance of ShellVoid.
     */
    static ShellClass& getInstance();

    virtual const std::string& getShellClassName() const override;

    virtual void shellPrint(std::ostream& stream) const override;
};

#endif	/* SHELLVOID_HPP */

