cmake_minimum_required(VERSION 3.5)
project(Server)

set(CMAKE_CXX_STANDARD 20)

macro(get_win_hex outvar)
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" ${outvar} ${CMAKE_SYSTEM_VERSION})
    math(EXPR ${outvar} "(${CMAKE_MATCH_1} << 8) + ${CMAKE_MATCH_2}" OUTPUT_FORMAT HEXADECIMAL)
endmacro()

if (WIN32)
    get_win_hex(winver)
    add_compile_definitions(_WIN32_WINNT=${winver})
    message("_WIN32_WINNT=${winver}")
endif ()

file(GLOB SOURCES src/* src/*/* src/*/*/*)

find_package(Boost 1.81.0 REQUIRED)
find_package(libpqxx REQUIRED)

include_directories(include ${Boost_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} libpqxx::pqxx)