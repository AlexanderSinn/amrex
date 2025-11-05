#include <AMReX_PODVector.H>
#include <AMReX_ParmParse.H>
#include <AMReX_REAL.H>
#include <AMReX_TinyProfiler.H>

namespace amrex
{
    Long mock_real_errno = 0;
    Long mock_real_crash_errno = 0;

    void print_Mock_err (double v) {
        ++mock_real_errno;
        TinyProfiler::PrintCallStack(std::cout);
        std::cout << mock_real_errno << " value " << v << " is out of bounds" << std::endl;
        if (mock_real_errno == mock_real_crash_errno) {
            throw v;
        }
    }

    void print_Mock_add_err (double a, double b) {
        ++mock_real_errno;
        TinyProfiler::PrintCallStack(std::cout);
        std::cout << mock_real_errno << " catastrophic cancelation: a= "
                << a << " b= " << b << " a+b= " << a+b
                << std::endl;
        if (mock_real_errno == mock_real_crash_errno) {
            throw (a+b);
        }
    }

}


namespace amrex::VectorGrowthStrategy
{
    Real growth_factor = 1.5_rt;

    // clamp user input to reasonable values
    constexpr Real min_factor = 1.001_rt;
    constexpr Real max_factor = 4._rt;

    namespace detail
    {
        void ValidateUserInput() {
            if (growth_factor < min_factor) {
                if (Verbose()) {
                    amrex::Print() << "Warning: user-provided vector growth factor is too small."
                                   << " Clamping to " << min_factor << ". \n";
                }
                growth_factor = min_factor;
            }

            if (growth_factor > max_factor) {
                if (Verbose()) {
                    amrex::Print() << "Warning: user-provided vector growth factor is too large."
                                   << " Clamping to " << max_factor << ". \n";
                }
                growth_factor = max_factor;
            }
        }
    }

    void Initialize () {
        ParmParse pp("amrex");
        pp.queryAdd("vector_growth_factor", growth_factor);
        pp.query("mock_real_crash_errno", amrex::mock_real_crash_errno);

        detail::ValidateUserInput();
    }

    void SetGrowthFactor (Real a_factor) {
        growth_factor = a_factor;
        detail::ValidateUserInput();
    }
}
