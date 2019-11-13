cmake_minimum_required(VERSION 3.0)
project(PolyMesh)

option(POLYMESH_ENABLE_ASSERTIONS "if true, enables assertions (even in RelWithDebug, not in Release)" ON)

file(GLOB_RECURSE SOURCE_FILES "src/*.cc")
file(GLOB_RECURSE HEADER_FILES "src/*.hh")

if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${SOURCE_FILES} ${HEADER_FILES})
endif()
add_library(polymesh STATIC ${SOURCE_FILES} ${HEADER_FILES})
set_property(TARGET polymesh PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(polymesh PUBLIC src/)

if (MSVC)
    target_compile_options(polymesh PUBLIC /MP)
else()
    target_compile_options(polymesh PRIVATE -Wall)
    target_link_libraries(polymesh PUBLIC -fuse-ld=gold)
endif()

if (POLYMESH_ENABLE_ASSERTIONS)
    target_compile_definitions(polymesh PUBLIC $<$<CONFIG:DEBUG>:POLYMESH_ENABLE_ASSERTIONS>)
    target_compile_definitions(polymesh PUBLIC $<$<CONFIG:RELWITHDEBINFO>:POLYMESH_ENABLE_ASSERTIONS>)
endif()

# optional libs:
if (TARGET glm)
    target_link_libraries(polymesh PUBLIC glm)
    target_compile_definitions(polymesh PUBLIC POLYMESH_SUPPORT_GLM)
    message(STATUS "[polymesh] enabled support for glm")
endif()

if (TARGET typed-geometry)
    target_link_libraries(polymesh PUBLIC typed-geometry)
    target_compile_definitions(polymesh PUBLIC POLYMESH_SUPPORT_TYPED_GEOMETRY)
    message(STATUS "[polymesh] enabled support for typed geometry")
endif()
