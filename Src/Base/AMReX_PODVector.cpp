#include <AMReX_PODVector.H>
#include <AMReX_ParmParse.H>
#include <AMReX_REAL.H>
#include <AMReX_TinyProfiler.H>
#include <AMReX_BLBackTrace.H>

#include <string>
#include <fstream>
#include <streambuf>
#include <map>
#include <sstream>

namespace amrex
{
    Long mock_real_errno = 0;
    Long mock_real_crash_errno = 0;

    struct Mock_err {
        Long nerr = 0;
        std::string first_err = "";
        std::string last_err = "";
        std::string tprof = "";
        std::string btslow = "";
    };

    std::map<std::string, Mock_err> real_rang_err;
    std::map<std::string, Mock_err> real_add_err;

    std::string get_backtrace_slow() {
        std::string file_name = "tmp_backtrace.txt";
        BLBackTrace::print_backtrace_info(file_name);
        std::ifstream t(file_name);
        std::string str((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());
        return str;
    }

    void add_err(std::map<std::string, Mock_err>& map, std::string const& msg, std::string&& bt) {
        auto & elem = map[bt];
        if (elem.nerr == 0) {
            std::cout << mock_real_errno << " Map size: " << map.size() << std::endl;
            elem.first_err = msg;
            std::stringstream ss;
            TinyProfiler::PrintCallStack(ss);
            elem.tprof = ss.str();
            elem.btslow = get_backtrace_slow();
        }
        ++elem.nerr;
        elem.last_err = msg;
    }

    std::string get_backtrace() {
        // std::string file_name = "tmp_backtrace.txt";
        // BLBackTrace::print_backtrace_info(file_name);
        // std::ifstream t(file_name);
        // std::string str((std::istreambuf_iterator<char>(t)),
        //                  std::istreambuf_iterator<char>());
        return BLBackTrace::get_backtrace_info();
    }


    void print_Mock_err (double v) {
        ++mock_real_errno;

        std::stringstream ss;
        ss << mock_real_errno << " value " << v << " is out of bounds";
        add_err(real_rang_err, ss.str(), get_backtrace());

        if (mock_real_errno == mock_real_crash_errno) {
            throw v;
        }
    }

    void print_Mock_add_err (double a, double b) {
        ++mock_real_errno;

        std::stringstream ss;
        ss << mock_real_errno << " catastrophic cancelation: a= "
                << a << " b= " << b << " a+b= " << a+b;
        add_err(real_add_err, ss.str(), get_backtrace());

        if (mock_real_errno == mock_real_crash_errno) {
            throw (a+b);
        }
    }

    void print_err_map (std::map<std::string, Mock_err>& map) {
        for (auto& [k, v] : map) {
            std::cout
                << "\n"
                << "Num errs: " << v.nerr << "\n\n"
                << "First err:\n" << v.first_err << "\n\n"
                << "Last err:\n" << v.last_err << "\n\n"
                << "Tprof stack:\n" << v.tprof << "\n\n"
                << "Backtrace:\n" << v.btslow << "\n\n\n\n\n\n" << std::endl;
        }
    }

    void finalize_mock_real () {
        print_err_map(real_rang_err);
        print_err_map(real_add_err);
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
