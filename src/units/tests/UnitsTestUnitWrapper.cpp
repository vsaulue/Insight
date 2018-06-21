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


#include <cmath>
#include <limits>

#include <catch.hpp>

#include "units/Units.hpp"
#include "units/Floats.hpp"

static constexpr double PI = 3.14159265359;

namespace U=Units;
namespace D=Dimensions;

template<typename T>
static constexpr bool almostEqual(T a, T b, T relDiff = 8*std::numeric_limits<T>::epsilon()) {
    double absDiff = std::abs(a-b);
    return (absDiff < relDiff * std::max(std::abs(a),std::abs(b)));
}

TEST_CASE("UnitWrapper<float,*>") {
    SECTION("Fundamental unit (angle)") {
        U::Float<decltype(U::degree)> deg(90);
        SECTION("Copy constructors") {
            SECTION("Same unit") {
                U::Float<decltype(U::degree)> deg2(deg);
                REQUIRE(deg.value == deg2.value);
            }

            SECTION("Different units") {
                U::Float<decltype(U::radian)> rad(deg);
                REQUIRE(almostEqual<float>(rad.value,PI/2));
            }
        }

        SECTION("operator=") {
            SECTION("Same unit") {
                U::Float<decltype(U::degree)> deg2(0);
                deg2 = deg;
                REQUIRE(deg.value == deg2.value);
            }

            SECTION("Different units") {
                U::Float<decltype(U::radian)> rad(0);
                rad = deg;
                REQUIRE(almostEqual<float>(rad.value,PI/2));
            }
        }

        SECTION("Unary operator-") {
            U::Float<decltype(U::degree)> deg2 = -deg;
            REQUIRE(deg2.value == -deg.value);
        }

        SECTION("operator+=") {
            SECTION("Same unit") {
                U::Float<decltype(U::degree)> deg2(45);
                deg+= deg2;
                REQUIRE(deg.value == 135);
            }

            SECTION("Different units") {
                U::Float<decltype(U::radian)> rad(PI/4);
                rad += deg;
                REQUIRE(almostEqual<float>(rad.value, 3*PI/4));
            }
        }

        SECTION("operator-=") {
            SECTION("Same unit") {
                U::Float<decltype(U::degree)> deg2(60);
                deg-= deg2;
                REQUIRE(deg.value == 30);
            }

            SECTION("Different units") {
                U::Float<decltype(U::radian)> rad(PI/3);
                rad -= deg;
                REQUIRE(almostEqual<float>(rad.value, -PI/6));
            }
        }

        SECTION("operator*=") {
            SECTION("No-unit") {
                U::Float<decltype(U::nounit)> constant(1.5);
                deg *= constant;
                REQUIRE(deg.value == 135);
            }

            SECTION("Unit ratio") {
                U::Float<decltype(U::radian / U::degree)> ratio(PI/30);
                deg *= ratio;
                REQUIRE(almostEqual<float>(deg.value, 540));
            }

            SECTION("Raw value") {
                deg *= 3.5;
                REQUIRE(deg.value == 315);
            }
        }

        SECTION("operator/=") {
            SECTION("No-unit") {
                U::Float<decltype(U::nounit)> constant(-3);
                deg /= constant;
                REQUIRE(deg.value == -30);
            }

            SECTION("Unit ratio") {
                U::Float<decltype(U::radian / U::degree)> ratio(PI/240);
                deg /= ratio;
                REQUIRE(almostEqual<float>(deg.value, 120));
            }

            SECTION("Raw value") {
                deg /= -0.5;
                REQUIRE(deg.value == -180);
            }
        }

        SECTION("operator+") {
            U::Float<decltype(U::degree)> deg2(-135);
            auto r1 = deg + deg2;
            REQUIRE(r1.value == -45);
            auto r2 = deg2 + deg;
            REQUIRE(r2.value == -45);
        }

        SECTION("operator-") {
            U::Float<decltype(U::degree)> deg2(150);
            auto r1 = deg - deg2;
            REQUIRE(r1.value == -60);
            auto r2 = deg2 - deg;
            REQUIRE(r2.value == 60);
        }

        SECTION("operator*") {
            SECTION("With unit types") {
                U::Float<decltype(U::meter/U::second)> speed(2.5);
                constexpr auto expectedUnit = U::meter / U:: second * U::degree;
                auto r1 = speed * deg;
                static_assert(decltype(r1)::Unit() == expectedUnit,"");
                REQUIRE(r1.value == 225);
                auto r2 = deg * speed;
                static_assert(decltype(r2)::Unit() == expectedUnit,"");
                REQUIRE(r2.value == 225);

            }

            SECTION("With raw values") {
                auto r1 = 0.5 * deg;
                static_assert(decltype(r1)::Unit() == decltype(deg)::Unit(),"");
                REQUIRE(r1.value == 45);
                auto r2 = deg * -2;
                static_assert(decltype(r2)::Unit() == decltype(deg)::Unit(),"");
                REQUIRE(r2.value == -180);
            }
        }

        SECTION("operator/") {
            SECTION("With unit types") {
                U::Float<decltype(U::meter/U::second[U::n2])> accel(-180);
                constexpr auto expectedUnit = U::meter / U:: second[U::n2] / U::degree;
                auto r1 = accel / deg;
                static_assert(decltype(r1)::Unit() == expectedUnit,"");
                REQUIRE(r1.value == -2);
                auto r2 = deg / accel;
                static_assert(decltype(r2)::Unit() == expectedUnit[-U::n1],"");
                REQUIRE(r2.value == -0.5);
            }

            SECTION("With raw values") {
                auto r1 = deg / 3;
                static_assert(decltype(r1)::Unit() == decltype(deg)::Unit(),"");
                REQUIRE(r1.value == 30);
                auto r2 = 360 / deg;
                static_assert(decltype(r2)::Unit() == decltype(deg)::Unit()[-U::n1],"");
                REQUIRE(r2.value == 4);
            }
        }

        SECTION("comparison operators") {
            SECTION("different units") {
                U::Float<decltype(U::radian)> rad(2.0);
                REQUIRE(deg < rad);
                REQUIRE_FALSE(rad < deg);
                REQUIRE(deg <= rad);
                REQUIRE_FALSE(rad <= deg);
                REQUIRE(rad >= deg);
                REQUIRE_FALSE(deg >= rad);
                REQUIRE(rad > deg);
                REQUIRE_FALSE(deg > rad);
            }

            SECTION("same unit & value") {
                U::Float<decltype(U::degree)> deg2(90);
                REQUIRE(deg == deg2);
                REQUIRE(deg2 == deg);
                REQUIRE_FALSE(deg != deg2);
                REQUIRE_FALSE(deg2 != deg);
            }
        }
    }

    SECTION("Multi-types, multi-units") {
        using FloatUnit = decltype(U::degree[U::n4]*U::meter[-U::n1]/U::second);
        U::Float<FloatUnit> floatVal(12345);
        using DoubleUnit = decltype(U::meter[U::n2]*U::second*U::radian[-U::n2]);
        U::Double<DoubleUnit> doubleVal(-1000);

        SECTION("Multiplication") {
            auto mult = floatVal * doubleVal;
            using MultExpectedUnit = decltype(U::degree[U::n4]*U::meter/U::radian[U::n2]);
            using MultUnit = typename decltype(mult)::Unit;
            static_assert(std::is_same<decltype(mult.value), double>::value,"");
            static_assert(MultUnit() == MultExpectedUnit(), "");
            static_assert(almostEqual<double>(MultUnit::ratio, meta::pow<4>(U::Degree::ratio)), "");
            REQUIRE(mult.value == floatVal.value * doubleVal.value);
        }

        SECTION("Division") {
            auto div = doubleVal / floatVal;
            using DivExpectedUnit = decltype(U::degree[-U::n4]*U::meter[U::n3]*U::radian[-U::n2]*U::second[U::n2]);
            using DivUnit = typename decltype(div)::Unit;
            static_assert(std::is_same<decltype(div.value), double>::value,"");
            static_assert(DivUnit() == DivExpectedUnit(), "");
            static_assert(almostEqual<double>(DivUnit::ratio, meta::pow<-4>(U::Degree::ratio)), "");
            REQUIRE(almostEqual<double>(div.value, doubleVal.value / floatVal.value));
        }
    }
}
