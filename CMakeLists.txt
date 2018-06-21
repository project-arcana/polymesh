cmake_minimum_required(VERSION 3.0)
project(PolyMesh)

file(GLOB_RECURSE SOURCE_FILES "src/*.cc")
file(GLOB_RECURSE HEADER_FILES "src/*.hh")

add_library(polymesh STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(polymesh PUBLIC src/)

if (MSVC)
    target_compile_options(polymesh PUBLIC /MP)
else()
    target_compile_options(polymesh PRIVATE -Wall -Werror -fPIC)
    target_compile_options(polymesh PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)
endif()
