cmake_minimum_required(VERSION 3.0)
project(PolyMesh)

file(GLOB_RECURSE SOURCE_FILES "src/*.cc")
file(GLOB_RECURSE HEADER_FILES "src/*.hh")

if(${CMAKE_VERSION} VERSION_GREATER "3.8.0") 
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${SOURCE_FILES} ${HEADER_FILES})
endif()
add_library(polymesh STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(polymesh PUBLIC src/)

if (MSVC)
    target_compile_options(polymesh PUBLIC /MP)
else()
    target_compile_options(polymesh PRIVATE -Wall -fPIC)
    target_link_libraries(polymesh PUBLIC -fuse-ld=gold)
endif()
