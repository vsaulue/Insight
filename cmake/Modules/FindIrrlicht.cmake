# Copyright (C) 2010-2017 celeron55, Perttu Ahola <celeron55@gmail.com>
# Copyright (C) 2017 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This file is a derivative work from a file of Minetest project. The original
# file is licensed under the GNU Lesser General Public License version 2.1.
# See <https://github.com/minetest/minetest>.

set(Irrlicht_BASEDIR "" CACHE PATH "Base directory of irrlicht")

if(NOT ${Irrlicht_BASEDIR} STREQUAL "")
    if(WIN32)
        if(${CMAKE_SIZEOF_VOID_P} EQUAL "8")
            set(Irrlicht_LIB_FOLDER "Win64")
        else()
            set(Irrlicht_LIB_FOLDER "Win32")
        endif()

        if(MSVC)
            set(Irrlicht_LIB_FOLDER "${Irrlicht_LIB_FOLDER}-VisualStudio")
            set(Irrlicht_STATIC_LIB_NAME "Irrlicht.lib")
        else()
            set(Irrlicht_LIB_FOLDER "${Irrlicht_LIB_FOLDER}-gcc")
            set(Irrlicht_STATIC_LIB_NAME "libIrrlicht.a")
        endif()

        find_file(Irrlicht_DLL NAMES Irrlicht.dll
                               PATHS "${Irrlicht_BASEDIR}/bin/${Irrlicht_LIB_FOLDER}"
                               DOC "Irrlicht dynamic library (for runtime)."
        )
    else()
        set(Irrlicht_LIB_FOLDER "Linux")
        set(Irrlicht_STATIC_LIB_NAME "libIrrlicht.a")
    endif()

    find_path(Irrlicht_INCLUDE_DIR NAMES irrlicht.h
                                   PATHS "${Irrlicht_BASEDIR}/include"
                                   NO_DEFAULT_PATH
    )

    find_library(Irrlicht_STATIC_LIB NAMES "${Irrlicht_STATIC_LIB_NAME}"
                                     PATHS "${Irrlicht_BASEDIR}/lib/${Irrlicht_LIB_FOLDER}"
                                     NO_DEFAULT_PATH
    )
else()
    find_path(Irrlicht_INCLUDE_DIR NAMES "irrlicht.h")
    find_library(Irrlicht_STATIC_LIB NAMES "libIrrlicht.a")
endif()

set(Irrlicht_INCLUDE_DIRS "${Irrlicht_INCLUDE_DIR}")
set(Irrlicht_LIBRARIES "${Irrlicht_STATIC_LIB}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Irrlicht DEFAULT_MSG
                                  Irrlicht_STATIC_LIB Irrlicht_INCLUDE_DIR)
mark_as_advanced(Irrlicht_LIB_FOLDER Irrlicht_INCLUDE_DIR Irrlicht_STATIC_LIB)