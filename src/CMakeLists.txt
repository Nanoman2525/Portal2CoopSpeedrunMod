cmake_minimum_required(VERSION 3.5)

project(coop_srm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


include_directories(include)
add_compile_definitions(PLUGIN_VERSION="v1.0")
set(SOURCES_COOP_SRM
    "src/coop_srm.cpp"
    "src/entry.cpp"
    "src/logger.cpp"
    "src/modules.cpp"
    "src/scanner.cpp"
)

add_library(coop_srm SHARED ${SOURCES_COOP_SRM})
if (UNIX)
    set_target_properties(coop_srm PROPERTIES COMPILE_FLAGS "-m32 -march=native" LINK_FLAGS "-m32")
endif (UNIX)
