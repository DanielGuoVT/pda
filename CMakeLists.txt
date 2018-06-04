cmake_minimum_required(VERSION 3.3)
project(Proteus)

list (APPEND CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
find_package(Z3 REQUIRED)
find_package(Graphviz REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG  "-Wall -fno-rtti -O0 -g")
set(SOURCE_FILES main.cpp FuncSummaryPass.cpp FuncSummaryPass.h PDA.cpp PDA.h Executor.cpp Executor.h Utils.cpp Utils.h PDA2Dot.cpp PDA2Dot.h)

add_executable(Proteus ${SOURCE_FILES})

include_directories(${LLVM_INCLUDE_DIRS} .)
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs
        support core irreader executionengine interpreter
        mc mcjit bitwriter x86codegen target)

target_link_libraries(Proteus ${llvm_libs})

if (NOT Z3_FOUND)
    message(WARNING "No Z3 found. Run \n\tcmake --build . && cmake ${CMAKE_SOURCE_DIR}")
else()
    include_directories(${Z3_INCLUDE_DIR})
    target_link_libraries(Proteus  ${Z3_LIBRARY})
endif()




message(STATUS "${llvm_libs}")