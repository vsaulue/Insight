/*
 * This file is part of Insight.
 * Copyright (C) 2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#ifndef IRRLICHT_PTR_HPP
#define IRRLICHT_PTR_HPP

#include <memory>

#include "irrlicht.h"

/**
 * Deleter for any type derived from irr::IReferenceCounted.
 *
 * This object is a valid Deleter for std::unique_ptr template parameter.
 */
struct IrrlichtDeleter {
public:
    void operator() (irr::IReferenceCounted* object) {
        object->drop();
    }
};

/**
 * Smart pointer for any Irrlicht type derived from irr::IReferenceCounted.
 */
template<typename T>
using irrlicht_ptr = std::unique_ptr<T, IrrlichtDeleter>;

#endif /* IRRLICHT_PTR_HPP */
