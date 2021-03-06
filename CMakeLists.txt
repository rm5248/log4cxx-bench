cmake_minimum_required(VERSION 3.14)
project(log4cxx_bench CXX)

find_package(Threads REQUIRED)
#find_package(benchmark CONFIG)
#if (NOT benchmark_FOUND)
#    message(STATUS "Using CMake Version ${CMAKE_VERSION}")
#    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.11.0")
#        # User can fetch googlebenchmark 			
#        message(STATUS "Downloading GoogleBenchmark")
#        include(FetchContent)
#        set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "")
#        # Do not build and run googlebenchmark tests
#        FetchContent_Declare(googlebenchmark
#            GIT_REPOSITORY https://github.com/google/benchmark.git
#            GIT_TAG v1.5.2)
#
#        FetchContent_MakeAvailable(googlebenchmark)
#    else()
#        message(FATAL_ERROR "GoogleBenchmark is missing. Use CMake >= 3.11 or download it")
#    endif()
#endif()

set(CMAKE_CXX_STANDARD 17)

find_package(fmt)

find_package(log4cxx 0.12)
add_executable(bench bench.cpp log4cxxbenchmarker.cpp )
target_include_directories(bench PRIVATE $<TARGET_PROPERTY:log4cxx,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries( bench PRIVATE fmt::fmt log4cxx Threads::Threads )
#target_compile_options(bench PRIVATE -fsanitize=leak -fsanitize=address)
#target_link_options(bench PRIVATE -fsanitize=leak -fsanitize=address)

#target_compile_options(bench PRIVATE -fsanitize=thread)
#target_link_options(bench PRIVATE -fsanitize=thread)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx.xml
                ${CMAKE_CURRENT_BINARY_DIR}/log4cxx.xml
                COPYONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx-async.xml
                ${CMAKE_CURRENT_BINARY_DIR}/log4cxx-async.xml
		COPYONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx-rotating.xml
                ${CMAKE_CURRENT_BINARY_DIR}/log4cxx-rotating.xml
		COPYONLY )

#add_executable(async_bench async_bench.cpp)
#target_link_libraries(async_bench PRIVATE spdlog::spdlog)

#add_executable(latency latency.cpp)
#target_link_libraries(latency PRIVATE benchmark::benchmark spdlog::spdlog)

#add_executable(formatter-bench formatter-bench.cpp)
#target_link_libraries(formatter-bench PRIVATE benchmark::benchmark spdlog::spdlog)
