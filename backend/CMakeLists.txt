cmake_minimum_required(VERSION 3.22)
project(TouristClubServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(PQXX_NO_SOURCE_LOCATION)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

include_directories(include ${PQXX_INCLUDE_DIRS})

set(SOURCES
    src/DatabaseManager.cpp
    src/AuthController.cpp
    src/TourManager.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Чисте лінкування без експериментальних хвостів
target_link_libraries(${PROJECT_NAME} PRIVATE ${PQXX_LIBRARIES})

message(STATUS "Project configured successfully. Ready to build!")