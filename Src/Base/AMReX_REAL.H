#ifndef AMREX_REAL_H_
#define AMREX_REAL_H_
#include <AMReX_Config.H>

#include <AMReX_Extension.H>

#include <iostream>
#include <utility>
#include <type_traits>
#include <limits>
#include <cmath>

#ifdef BL_USE_FLOAT
#  undef BL_USE_DOUBLE
#  undef BL_USE_FLOAT
/*
  The macro BL_USE_FLOAT indicates that C++ floating-point calculations should
  use "float" variables and Fortran floating-point calculations should use
  "real*4" variables.  One of BL_USE_FLOAT or BL_USE_DOUBLE must always be
  defined when compiling and using AMReX.
*/
#  define BL_USE_FLOAT 1
#else
#  undef  BL_USE_FLOAT
#  undef  BL_USE_DOUBLE
/*
  The macro BL_USE_DOUBLE indicates that C++ floating-point calculations
  should use "double" variables and Fortran floating-point calculations
  should use "real*8" variables.  One of BL_USE_FLOAT or BL_USE_DOUBLE must
  always be defined when compiling and using AMReX.
*/
#  define BL_USE_DOUBLE 1
#endif

#if !defined(BL_LANG_FORT)
#ifdef __cplusplus
#include <cfloat>
#else
#include <float.h>
#endif

/*
  Real is a typedef specifying the precision of the floating-point
  calculations in C++ code.  It will be either float or double
  depending upon which of the macros BL_USE_FLOAT or
  BL_USE_DOUBLE, respectively, is defined during compilations.  For
  portability, you should write floating-point code in terms of this
  typedef, instead of using float or double directly.

  Note that exactly one of these macros must be defined
  when compiling any module that uses floating-point.
*/

#ifdef BL_USE_FLOAT

// We need to define these to get around a CUDA 9.2 bug that breaks std::numeric_limits
#define AMREX_REAL_MIN     FLT_MIN
#define AMREX_REAL_MAX     FLT_MAX
#define AMREX_REAL_LOWEST (-FLT_MAX)
#else
#define AMREX_REAL_MIN     DBL_MIN
#define AMREX_REAL_MAX     DBL_MAX
#define AMREX_REAL_LOWEST (-DBL_MAX)
#endif

#ifdef __cplusplus
namespace amrex {

    #define TEMPLATE_TWO_TYPE  template<class T, class U, std::enable_if_t< \
        ( \
            std::is_same_v<T, MockReal> || std::is_same_v<U, MockReal> \
        ) && ( \
            std::is_same_v<T, MockReal> || std::is_same_v<T, double> || std::is_same_v<T, int> || \
            std::is_same_v<T, float> || std::is_same_v<T, long long> || std::is_same_v<T, unsigned int> || \
            std::is_same_v<T, unsigned long long> || std::is_same_v<T, bool> \
        ) && ( \
            std::is_same_v<U, MockReal> || std::is_same_v<U, double> || std::is_same_v<U, int> || \
            std::is_same_v<U, float> || std::is_same_v<U, long long> || std::is_same_v<U, unsigned int> || \
            std::is_same_v<U, unsigned long long> || std::is_same_v<U, bool> \
        ), int> = 0>


    void print_Mock_err (double v);
    void print_Mock_add_err (double a, double b);

    struct MockReal {
        double val = 0.;

        constexpr MockReal() = default;
        constexpr MockReal (const MockReal&) = default;
        constexpr MockReal& operator= (const MockReal&) = default;

        constexpr MockReal (double a) : val{a} {check_val();}
        constexpr MockReal (float a) : val{static_cast<double>(a)} {check_val();}
        constexpr MockReal (long double a) : val{static_cast<double>(a)} {check_val();}
        constexpr MockReal (int a) : val{static_cast<double>(a)} {check_val();}
        constexpr MockReal (long a) : val{static_cast<double>(a)} {check_val();}
        constexpr MockReal (long long a) : val{static_cast<double>(a)} {check_val();}
        constexpr MockReal (unsigned int a) : val{static_cast<double>(a)} {check_val();}
        constexpr MockReal (unsigned long a) : val{static_cast<double>(a)} {check_val();}
        constexpr MockReal (unsigned long long a) : val{static_cast<double>(a)} {check_val();}
        constexpr MockReal (bool a) : val{static_cast<double>(a)} {check_val();}

        static constexpr MockReal lowest(){ return -FLT_MAX; };
        static constexpr MockReal max(){ return FLT_MAX; };
        static constexpr MockReal min(){ return FLT_MIN; };
        static constexpr MockReal epsilon(){ return FLT_EPSILON; };

        constexpr void check_val() const {
            double a = val >= 0. ? val : -val;
            if (a != 0 &&
                a != double(FLT_MAX) &&
                a != double(FLT_MIN) &&
                (a > 3.e38 || a < 2.e-38)) {
                print_Mock_err(a);
            }
        }

        static constexpr MockReal unchecked_add(const MockReal& a, const MockReal& b) {
            return MockReal{a.val + b.val};
        }

        static constexpr MockReal unchecked_sub(const MockReal& a, const MockReal& b) {
            return MockReal{a.val - b.val};
        }

        static constexpr void check_add(double a, double b) {
            if (std::abs(a + b) * 1.e6 < std::abs(a - b)) {
                print_Mock_add_err(a, b);
            }
        }

        constexpr operator double () {
            return val;
        }

        constexpr operator double () const {
            return val;
        }

        constexpr MockReal& operator+= (const MockReal& a) {
            check_add(val, a.val);
            val += a.val;
            check_val();
            return *this;
        }

        constexpr MockReal& operator-= (const MockReal& a) {
            check_add(val, -a.val);
            val -= a.val;
            check_val();
            return *this;
        }

        constexpr MockReal& operator*= (const MockReal& a) {
            val *= a.val;
            check_val();
            return *this;
        }

        constexpr MockReal& operator/= (const MockReal& a) {
            val /= a.val;
            check_val();
            return *this;
        }

        constexpr MockReal operator- () const {
            return MockReal{-val};
        }

        TEMPLATE_TWO_TYPE
        constexpr friend MockReal operator+ (const T& a, const U& b) {
            check_add(MockReal{a}.val, MockReal{b}.val);
            return MockReal{a}.val + MockReal{b}.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend MockReal operator- (const T& a, const U& b) {
            check_add(MockReal{a}.val, -MockReal{b}.val);
            return MockReal{a}.val - MockReal{b}.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend MockReal operator* (const T& a, const U& b) {
            return MockReal{a}.val * MockReal{b}.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend MockReal operator/ (const T& a, const U& b) {
            return MockReal{a}.val / MockReal{b}.val;
        }

        friend std::ostream& operator<<(std::ostream& os, const MockReal& a) {
            return os << a.val;
        }

        friend std::istream& operator>>(std::istream& is, MockReal& a) {
            return is >> a.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend bool operator== (const T& a, const U& b) {
            return MockReal{a}.val == MockReal{b}.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend bool operator!= (const T& a, const U& b) {
            return MockReal{a}.val != MockReal{b}.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend bool operator< (const T& a, const U& b) {
            return MockReal{a}.val < MockReal{b}.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend bool operator> (const T& a, const U& b) {
            return MockReal{a}.val > MockReal{b}.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend bool operator<= (const T& a, const U& b) {
            return MockReal{a}.val <= MockReal{b}.val;
        }

        TEMPLATE_TWO_TYPE
        constexpr friend bool operator>= (const T& a, const U& b) {
            return MockReal{a}.val >= MockReal{b}.val;
        }
    };

    template<class T>
    struct IsFloatingPoint : std::is_floating_point<T> {};

    template<>
    struct IsFloatingPoint<MockReal> : std::true_type {};

    template<class T>
    constexpr bool IsFloatingPoint_v = IsFloatingPoint<MockReal>::value;

    /** Floating Point Type for Fields
     *
     * This is a floating point type used by default for fields such as
     * FArrayBox and MultiFab classes. This can be changed at compile time to
     * run double, single or experimental floating point precisions for field
     * computations and storage.
     */
    using Real = MockReal;

    /** Floating Point Type for Particles
     *
     * This is a floating point type used by default for ParticleContainer and
     * related particle classes. This can be changed at compile time to run
     * double, single or experimental floating point precisions for particle
     * computations and storage.
     */
    using ParticleReal = MockReal;

/** Floating point literals for AMReX precision.
 *
 * This namespace can be used to write typed numeric literals in expressions.
 */
inline namespace literals {

    /** @{
      C++ user literals ``_rt`` &  ``_prt`` for short-hand notations

      Use this to properly add types to constant such as
      ```C++
      auto const mypi = 3.14_rt;
      auto const sphere_volume = 4_rt / 3_rt * pow(r, 3) * mypi;
      ```
    */
    constexpr Real
    operator""_rt( long double x )
    {
        return Real( x );
    }

    constexpr Real
    operator""_rt( unsigned long long int x )
    {
        return Real( x );
    }

    constexpr ParticleReal
    operator""_prt( long double x )
    {
        return ParticleReal( x );
    }

    constexpr ParticleReal
    operator""_prt( unsigned long long int x )
    {
        return ParticleReal( x );
    }
    /// @}
} // namespace literals
} // namespace amrex


template<>
struct std::numeric_limits<amrex::MockReal> : std::numeric_limits<float> {};

#endif

#ifdef AMREX_SINGLE_PRECISION_PARTICLES
typedef amrex::MockReal amrex_real; // NOLINT(modernize-use-using)
typedef amrex::MockReal amrex_particle_real; // NOLINT(modernize-use-using)
#else
typedef amrex::MockReal amrex_real; // NOLINT(modernize-use-using)
typedef amrex::MockReal amrex_particle_real; // NOLINT(modernize-use-using)
#endif

#endif /* !BL_LANG_FORT */

#endif /*BL_REAL_H*/
