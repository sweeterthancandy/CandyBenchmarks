


# Getting Started Windows
        
        git clone https://github.com/sweeterthancandy/CandyBenchmarks.git
        cd CandyBenchmarks
        mkdir deps
        cd deps
        git clone https://gitlab.com/libeigen/eigen.git
        git clone https://github.com/google/googletest.git
        git clone https://github.com/google/benchmark.git
        cd ..
        cmake -B build -DCMAKE_BUILD_TYPE=Release -DBoost_INCLUDE_DIR=C:\work\boost_1_77_0\boost_1_77_0
        REM open build\ps.sln and build
