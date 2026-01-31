# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TradingSimulator)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# List all source and header files for the project
set(SOURCES
    main.cpp       # Entry point of the application
    Broker.cpp     # Implementation for Broker class
    user.cpp       # Implementation for User class
    marketData.cpp # Implementation for MarketData class
    Broker.h       # Header for Broker class
    user.h         # Header for User class
    marketData.h   # Header for MarketData class
    DBManager.cpp  # Implementation for DBManager class (SQLite operations)
    DBManager.h    # Header for DBManager class
    strategyBase.h
)

# Create an executable target from the listed source files
add_executable(TradingSimulator ${SOURCES})

# Include the current directory for header file resolution
target_include_directories(TradingSimulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Find SQLite3 library installed on the system
find_package(SQLite3 REQUIRED)

# Link SQLite3 library to the executable
target_link_libraries(TradingSimulator PRIVATE SQLite::SQLite3)
