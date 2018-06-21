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

#include <catch.hpp>

#include "units/Dimensions.hpp"
#include "units/Floats.hpp"
#include "units/Units.hpp"
#include "units/UnitSystem.hpp"

namespace D = Dimensions;

struct AngleA : public Units::UnitIdentifier<D::Angle> {
    static constexpr char symbol[] = "Aa";
    static constexpr double ratio = 0.125;
};

struct AngleB : public Units::UnitIdentifier<D::Angle> {
    static constexpr char symbol[] = "Ab";
    static constexpr double ratio = 2;
};

struct LengthA : public Units::UnitIdentifier<D::Length> {
    static constexpr char symbol[] = "La";
    static constexpr double ratio = 2;
};

struct LengthB : public Units::UnitIdentifier<D::Length> {
    static constexpr char symbol[] = "Lb";
    static constexpr double ratio = 1;
};

struct MassA : public Units::UnitIdentifier<D::Mass> {
    static constexpr char symbol[] = "Ma";
    static constexpr double ratio = 8;
};

struct MassB : public Units::UnitIdentifier<D::Mass> {
    static constexpr char symbol[] = "Mb";
    static constexpr double ratio = 0.5;
};

struct TimeA : public Units::UnitIdentifier<D::Time> {
    static constexpr char symbol[] = "Ta";
    static constexpr double ratio = 0.5;
};

struct TimeB : public Units::UnitIdentifier<D::Time> {
    static constexpr char symbol[] = "Tb";
    static constexpr double ratio = 0.25;
};

struct TagsSystemA {
    using Angle = AngleA;
    using Length = LengthA;
    using Mass = MassA;
    using Time = TimeA;
};

struct SystemA : public Units::UnitSystem<TagsSystemA> {

};

struct TagsSystemB {
    using Angle = AngleB;
    using Length = LengthB;
    using Mass = MassB;
    using Time = TimeB;
};

struct SystemB : public Units::UnitSystem<TagsSystemB> {

};

TEST_CASE("Unit systems.") {
    Units::Double<SystemA::Force> forceA(123);

    SECTION("Conversion to other system.") {
        auto forceB = Units::systemConvert<SystemB>(forceA);
        static_assert(Units::UnitConverter<SystemB,SystemA::Force>() == SystemB::Force(),"");
        static_assert(decltype(forceB)::Unit() == SystemB::Force(),"");
        REQUIRE(forceB.value == 123.0/8);
    }

    SECTION("Conversion to same system.") {
        auto force2 = Units::systemConvert<SystemA>(forceA);
        static_assert(Units::UnitConverter<SystemA,SystemA::Force>() == SystemA::Force(),"");
        static_assert(decltype(force2)::Unit() == decltype(forceA)::Unit(),"");
        REQUIRE(force2.value == forceA.value);
    }
}
