cmake_minimum_required(VERSION 3.26)
project(Server)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES src/* src/*/* src/*/*/*)

find_package(Boost 1.81.0 REQUIRED)
find_package(libpqxx REQUIRED)

include_directories(include ${Boost_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} libpqxx::pqxx)