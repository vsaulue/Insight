# Insight - Unit library

C++ library for [dimensional analysis](https://en.wikipedia.org/wiki/Dimensional_analysis) at compile time, and implicit conversion of values with different units.

# Usage

The goal of this library is to define:
- unit systems.
- a wrapper for existing generic data types (scalars, vectors, matrices), which also holds a unit.

This enables the compiler to:
- generate an error if a C++ expression is not compliant with dimensional analysis.
- automatically convert values in assignments when needed.

## Quick example

This first example uses the library wrapper for `float` and `double` types, and a variant of the [SI Unit system](https://en.wikipedia.org/wiki/International_System_of_Units) used in the main project.

Given the following values:
```
#include "units/Floats.hpp"
#include "units/SI.hpp"

Float<SI::Time> seconds(3);
Float<SI::AngularVelocity> rad_per_second(-7);
```
The following operation is valid:
```
Float<SI::Angle> rad = rad_per_second * second; //  internal value of "rad" is -21.
auto rad2 = rad_per_second * seconds; // should be equivalent.
```
The following assignment will implicitly convert rad/s into deg/s:
```
using DegPerSecondUnit = decltype(Units::degree/Units::second);
Float<DegPerSecondUnit> deg = rad_per_second; // <-- automatically multiply by (180/PI),
```
And the following lines will not compile, as they do not respect dimensional analysis constraints:
```
// The following lines will generate compile time errors.
auto foo = seconds + rad_per_second // Error: incompatible dimension in the sum.
Float<SI::Length> distance = rad_per_second / seconds; // Error: invalid assigment.
```

## Unit systems

### Fundamental dimensions

The library currently uses 4 fundamental dimensions: angle, length, mass, time. They are defined in [Dimensions.hpp](include/units/Dimensions.hpp). From a C++ perspective, they are empty types used as tags. Declaring new dimensions can be done in two lines:

```
// Defining "electric current" dimension (common unit: ampere).
struct ElectricCurrent {};
constexpr FundamentalDim<ElectricCurrent> electric_current;
```

In order to use them it in a unit system, the template `UnitWrapper` must be specialized in `units/UnitSystem.hpp`:

```
template<typename System>
struct UnitSystemMapper<System,D::ElectricCurrent> {
	using type = typename System::UnitTags::ElectricCurrent;
};
```

### Units

The library defines some common units in [Units.hpp](include/units/Units.hpp). Other units can anywhere in the project. A new unit can be defined like this:

```
/** Identifier of hour unit (time). */
struct Hour : UnitIdentifier<D::Time> {
    static constexpr char symbol[] = "h";
    static constexpr double ratio = Second::ratio / 3600.0;
};
/** Instance of the type representing values in hours. */
constexpr FundamentalUnit<Hour> hour;
```

Important parts:
- `UnitIdentifier<D::Time>` indicates that the units represents a `Time` dimension (only fundamental dimensions are allowed).
- `symbol[] = "h";` defines a string used to print the unit.
- `ratio = Second::ratio / 3600.0;` used to convert this unit to any other unit representing `Time` dimension.

### Unit systems

First, unit systems are a convenient way to derive compound units (speed, torque...) from a set of fundamental units. See `units/SI.hpp` to see how a new unit system can quickly be defined.

Second, unit systems are a convenient way to "project" any value into a specified unit system. Example:

```
#include "units/UnitSystem.hpp"
#include "units/SI.hpp"

Float<SomeVelocityUnit> value(12345); // SomeVelocityUnit dimension : Length / Time.
auto meter_per_second = systemConvert<SI>(value); // converted value Float<SI::Speed>.
```

To declare new compound units in all unit system, simply add an typedef in UnitSystem template class in [UnitSystem.hpp](include/units/UnitSystem.hpp). Here is how acceleration unit is defined for example:

```
using Acceleration = decltype(Length() / Time()[n2]);
```

This is a case where manipulating units (or dimensions) as constexpr expressions is way more readable than raw types and metaprogramming tricks.

## Using other existing data types (vectors, matrices)

The library provides the `UnitWrapper<DataType,Unit>` and `BasicUnitWrapper<DataType,Unit>` template class to wrap existing C++ types into a (value + unit) wrapper. The `Float` or `Double` types used previously are just aliases of this wrapper, specialized for `float` or `double` types.

The `UnitWrapper<DataType,Unit>` provides multiple features:
- inherited constructors from `DataType`.
- a `value` public member, which is the wrapped, raw value.
- assignment operator, which can perform implicit conversion to/from any compatible unit.
- overloaded binary operators `*` `/` `+` `-`, comparison operators, assignment operators `*=` `/=` `+=` `-=`.

The operators will work with wrappers of different types: if the following expressions are valid:
```
btMatrix3x3 mat; // Assume valid initialization
btVector3 in; // Assume valid initialization

btVector3 out = mat * (1.5 * out);
```

Then by just defining the following wrappers:
```
    template<typename Unit>
    struct UnitWrapper<btVector3,Unit> : public BasicUnitWrapper<btVector3,Unit> {
    private:
        using Base = BasicUnitWrapper<btVector3,Unit>;
    public:
        using Base::BasicUnitWrapper;
    }
    // same with btMatrix3x3
```

The following will work:

```
UnitWrapper<btMatrix3x3,SI::Speed> uMat(mat);
UnitWrapper<btVector3,SI::Time> uIn(in);

auto out1 = uMat * uIn;
UnitWrapper<btVector3,SI::Length> out2 = uMat * uIn;
```

The main project uses this feature to wrap existing data structure from the Bullet physics engine. Have a look into the physics library for more examples.