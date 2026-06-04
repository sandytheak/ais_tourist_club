cmake_minimum_required(VERSION 3.10)
project(TouristClubServer)

set(CMAKE_CXX_STANDARD 17)

# Знаходимо бібліотеку потоків
find_package(Threads REQUIRED)

# Твої файли
set(SOURCES
    src/DatabaseManager.cpp
    src/AuthController.cpp
    src/TourManager.cpp
    src/BookingManager.cpp
    src/AdminControl.cpp
    src/SystemLog.cpp
    src/Users.cpp
    src/AuthUI.cpp
    src/TouristDashboard.cpp
    src/AgencyPanel.cpp
    src/AdminPanel.cpp
    src/TechMonitor.cpp
    src/main.cpp
)

include_directories(include)

add_executable(TouristClubServer ${SOURCES})

# Обов'язково додаємо Threads::Threads до лінкера
target_link_libraries(TouristClubServer pqxx pq Threads::Threads)