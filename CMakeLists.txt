cmake_minimum_required(VERSION 3.22)
project(DoJaject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(Boost_LIBRARY_DIRS ${BOOST_ROOT}/libs)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_NO_WARN_NEW_VERSIONS ON)

find_package(Boost 1.79.0 REQUIRED COMPONENTS system filesystem)

add_executable(DoJaject src/DoJaject.cpp)
include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(DoJaject PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(DoJaject PRIVATE Boost::filesystem Boost::system "-static-libgcc" "-static-libstdc++" "-static")