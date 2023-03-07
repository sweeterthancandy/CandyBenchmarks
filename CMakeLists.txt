cmake_minimum_required(VERSION 3.6)

project(candy-benchmarks)

enable_testing()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(deps/googletest)


include_directories(${gtest_SOURCE_DIR})
include_directories(${gtest_SOURCE_DIR}/include)

add_subdirectory(deps/benchmark)

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

print_all_variables()

include_directories(deps/benchmark/include)
include_directories(build/deps/benchmark/include)




if( WIN32 )
  #find_package (Eigen3 3.3 REQUIRED NO_MODULE)
  include_directories( deps/eigen )
  include_directories( deps/eigen/Eigen )



  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(BUILD_SHARED_LIBS TRUE)
  
  set(Boost_USE_STATIC_LIBS        ON) # only find static libs
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME    OFF)
  
  set(BOOST_INCLUDEDIR C:/work/modular-boost )
  set(BOOST_LIBRARYDIR C:/work/modular-boost/stage/lib )

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
else()
  add_definitions(-DBOOST_LOG_DYN_LINK)

  if(0)
          set(CMAKE_CXX_FLAGS "-std=c++14 -frtti -fno-omit-frame-pointer -ggdb3 -mavx -mavx2 -fno-inline ")
  else()
          set(CMAKE_CXX_FLAGS "-std=c++14 -frtti  -Wno-switch")

          set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")
          set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -pedantic")
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")
          #for eigen
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-misleading-indentation")
          
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -mavx2")

          #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
  endif()


  #add_definitions(-D_GLIBCXX_ASSERTIONS)

endif()


#add_executable( benchmarks benchmarks.cpp)
#target_link_libraries(benchmarks ${Boost_LIBRARIES} benchmark::benchmark )


file(GLOB BENCHMARK_SOURCES "*.cpp")
foreach(src ${BENCHMARK_SOURCES})
        get_filename_component(exe_name ${src} NAME_WE)
        message(STATUS "src=${src}, exe=${exe_name}")
        add_executable( ${exe_name} ${src})
        target_link_libraries(${exe_name} ${Boost_LIBRARIES} benchmark::benchmark )
endforeach()


