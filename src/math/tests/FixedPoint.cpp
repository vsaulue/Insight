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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "FixedPoint.hpp"

#define TEST_EQUAL(a, b)     \
    REQUIRE((a) == (b));        \
    REQUIRE((b) == (a));        \
    REQUIRE_FALSE((a) != (b));  \
    REQUIRE_FALSE((b) != (a))

#define TEST_NOT_EQUAL(a, b)     \
    REQUIRE((a) != (b));        \
    REQUIRE((b) != (a));        \
    REQUIRE_FALSE((a) == (b));  \
    REQUIRE_FALSE((b) == (a))

#define TEST_ADD(a, b, res) \
    TEST_EQUAL((a) + (b), (res)); \
    TEST_EQUAL((b) + (a), (res))

#define TEST_SUB(a, b, res) \
    TEST_EQUAL((a) - (b), (res)); \
    TEST_EQUAL((b) - (a), (-res))

TEST_CASE("Equality & inequality between Fixed64.") {

    GIVEN("Two Fixed64<16> built from the same int64_t.") {
        int64_t value = 123987;
        Fixed64<16> a(value), b(value);

        TEST_EQUAL(a, b);
    }

    GIVEN("A Fixed64<16> and a Fixed64<32> built from the same int64_t.") {
        int64_t value = -3;
        Fixed64<16> a(value), b(value);

        TEST_EQUAL(a, b);
    }

    GIVEN("Two Fixed64<40> with different values.") {
        Fixed64<40> a(1), b(-1);

        TEST_NOT_EQUAL(a, b);
    }

    GIVEN("A Fixed<0> and Fixed<1> (same binary value, logically different).") {
        Fixed64<0> a(1);
        Fixed64<1> b;
        b.value = 1;

        TEST_NOT_EQUAL(a, b);
    }

    GIVEN("A Fixed<62>, and it's rounded value in a Fixed<8>.") {
        Fixed64<62> a(1);
        a.value |= 1;
        Fixed64<8> b(1);

        TEST_NOT_EQUAL(a, b);
    }
}

TEST_CASE("Initialization of Fixed64.") {

    GIVEN("Two Fixed64 built from equal int & double.") {
        int64_t iValue = -159860;
        double fValue(iValue);

        Fixed64<17> fromFloat(fValue);
        Fixed64<0> fromInt;
        fromInt.value = iValue;
        TEST_EQUAL(fromFloat, fromInt);
    }

    GIVEN("A Fixed64 built with a copy constructor.") {
        Fixed64<41> source(-7592.5);
        Fixed64<41> dest(source);

        REQUIRE(source == dest);
    }

    GIVEN("A Fixed64 modified after being used in a copy constructor.") {
        int64_t iValue = 12345;

        Fixed64<15> source(12345);
        Fixed64<15> dest(source);

        source.value = 54321;

        REQUIRE(dest == Fixed64<15>(iValue));
    }
}

TEST_CASE("Addition of numbers.") {

    GIVEN("-1 + 1") {
        TEST_ADD(Fixed64<16>(1), Fixed64<16>(-1), Fixed64<16>(0));
    }

    GIVEN("-3.75 + 657.125") {
        TEST_ADD(Fixed64<47>(-3.75), Fixed64<47>(657.125), Fixed64<47>(653.375));
    }
}

TEST_CASE("Negation of numbers.") {

    GIVEN("A positive number.") {
        double number = 1234.0625;
        TEST_EQUAL(Fixed64<50>(number), -Fixed64<50>(-number));
    }

    GIVEN("A negative number.") {
        double number = -0.75;
        TEST_EQUAL(Fixed64<2>(number), -Fixed64<2>(-number));
    }

    GIVEN("Zero.") {
        double number = 0;
        TEST_EQUAL(Fixed64<30>(number), -Fixed64<30>(-number));
    }
}

TEST_CASE("Substraction of numbers.") {

    GIVEN("A number minus itself.") {
        TEST_SUB(Fixed64<1>(123456789.5), Fixed64<1>(123456789.5), Fixed64<1>(0));
    }

    GIVEN("Two positive numbers.") {
        TEST_SUB(Fixed64<25>(1000000000), Fixed64<25>(0.125), Fixed64<25>(999999999.875));
    }

    GIVEN("Numbers with different signs.") {
        TEST_SUB(Fixed64<20>(-452.375), Fixed64<20>(255.5), Fixed64<20>(-707.875));
    }
}

TEST_CASE("Operator += and -=.") {

    GIVEN("A number added, then substracted, to another number.") {
        Fixed64<7> x0(-74859.375);
        Fixed64<7> x(x0);
        Fixed64<7> y(1234567891011);

        x+= y;
        x-= y;

        TEST_EQUAL(x, x0);
    }

    GIVEN("A number substracted, then added, to another number.") {
        Fixed64<7> x0(-9876543210.125);
        Fixed64<7> x(x0);
        Fixed64<7> y(4.75);

        x-= y;
        x+= y;

        TEST_EQUAL(x, x0);
    }
}

TEST_CASE("FixedPoint.toDoudle") {
    GIVEN("A double, converted to Fixed, then casted agin into double.") {
        double startVal(3.375);
        Fixed64<35> fixedVal(startVal);
        double resultVal = fixedVal.toDouble();

        REQUIRE(resultVal == startVal);
    }

    GIVEN("A Fixed, converted to double, then casted agin into Fixed.") {
        Fixed64<35> startVal(3.375);
        double doubleVal = startVal.toDouble();
        Fixed64<35> resultVal(doubleVal);

        TEST_EQUAL(resultVal,  startVal);
    }
}