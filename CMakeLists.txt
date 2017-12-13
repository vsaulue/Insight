# This file is part of Insight.
# Copyright (C) 2017 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
#
# Insight is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Insight is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Insight.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Insight)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(Insight_VERSION_MAJOR 0)
set(Insight_VERSION_MINOR 0)
set(Insight_VERSION_PATCH_LEVEL )
set(Insight_VERSION_EXTRA -dev)

set(Insight_VERSION ${Insight_VERSION_MAJOR}.${Insight_VERSION_MINOR})

if(NOT ("${Insight_VERSION_PATCH_LEVEL}" MATCHES " *"))
    set(Insight_VERSION ${Insight_VERSION}.${Insight_VERSION_PATCH_LEVEL})
endif()

if(NOT ("${Insight_VERSION_EXTRA}" MATCHES " *"))
    set(Insight_VERSION ${Insight_VERSION}.${Insight_VERSION_EXTRA})
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()

find_path(Catch_INCLUDE_DIRS NAMES "catch.hpp"
    PATHS "external/catch-1.3.4/include"
    NO_DEFAULT_PATH
)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${Catch_INCLUDE_DIRS})

find_package(Irrlicht REQUIRED)
add_library(Irrlicht STATIC IMPORTED)
set_target_properties(Irrlicht PROPERTIES
    IMPORTED_LOCATION ${Irrlicht_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES ${Irrlicht_INCLUDE_DIRS}
)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_custom_target(run-tests)

add_subdirectory(src)