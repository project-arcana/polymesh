cmake_minimum_required(VERSION 3.0)
project(PolyMesh)

if (NOT TARGET clean-core)
    message(FATAL_ERROR "[${PROJECT_NAME}] clean-core must be available")
endif()

option(POLYMESH_ENABLE_ASSERTIONS "if true, enables assertions (even in RelWithDebug, not in Release)" ON)

file(GLOB_RECURSE SOURCES "src/*.cc")
file(GLOB_RECURSE HEADERS "src/*.hh")

arcana_add_library(POLYMESH polymesh SOURCES HEADERS)
set_property(TARGET polymesh PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(polymesh PUBLIC src/)

if (MSVC)
    target_compile_options(polymesh PUBLIC /MP)
else()
    target_compile_options(polymesh PRIVATE -Wall)
    if (LINUX)
        target_link_libraries(polymesh PUBLIC -fuse-ld=gold)
    endif()
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
