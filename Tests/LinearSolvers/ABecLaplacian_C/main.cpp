
#include <AMReX.H>
#include "MyTest.H"

int main (int argc, char* argv[])
{
    amrex::Initialize(argc, argv);

    {
        BL_PROFILE("main");
        MyTest mytest;

        for(int i=0; i<100; ++i) {
            mytest.solve();
            for (int ilev = 0; ilev <= mytest.max_level; ++ilev)
            {
                mytest.solution[ilev].setVal(0.0);
            }
        }
        mytest.writePlotfile();
    }

    amrex::Finalize();
}
