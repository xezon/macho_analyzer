cmake_minimum_required(VERSION 3.16)

project(macho_analyzer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Explicitly set output directories for all build types
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    add_compile_options(/wd26819 /wd26498 /wd26800 /wd26495)
endif()

# Configure LIEF options
set(LIEF_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIEF_INSTALL OFF CACHE BOOL "" FORCE)
set(LIEF_C_API OFF CACHE BOOL "" FORCE)
set(LIEF_PYTHON_API OFF CACHE BOOL "" FORCE)
set(LIEF_TESTS OFF CACHE BOOL "" FORCE)
set(LIEF_ART OFF CACHE BOOL "" FORCE)
set(LIEF_DEX OFF CACHE BOOL "" FORCE)
set(LIEF_VDEX OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    lief
    GIT_REPOSITORY https://github.com/lief-project/LIEF.git
    GIT_TAG        0.15.1
)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        11.0.2
)

FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG        v3.2.1
)

FetchContent_MakeAvailable(lief json fmt cxxopts)

include(cmake/xllvmdemangler.cmake)

# Create macho_analyzer executable
add_executable(macho_analyzer 
    src/analyzer_main.cpp
    src/demangle.cpp
    src/demangle.h
    src/macho_parser.cpp
    src/macho_parser.h
    src/macho_symbol.cpp
    src/macho_symbol.h
    src/macho_section.cpp
    src/macho_section.h
    src/macho_segment.cpp
    src/macho_segment.h
    src/utility.cpp
    src/utility.h
)

target_include_directories(macho_analyzer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(macho_analyzer
    PRIVATE
        LIEF::LIEF
        nlohmann_json::nlohmann_json
        fmt::fmt
        cxxopts::cxxopts
        XLLVMDemangler
)
