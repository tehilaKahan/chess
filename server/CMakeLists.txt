cmake_minimum_required(VERSION 3.16)
project(ChessServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# נסה למצוא ixwebsocket, אם לא - השתמש בנתיב ידני
find_package(ixwebsocket CONFIG QUIET)

if(NOT ixwebsocket_FOUND)
    # נתיב ידני ל-ixwebsocket
    set(IXWEBSOCKET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/installed/x64-windows/include")
    set(IXWEBSOCKET_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/installed/x64-windows/lib")
endif()

# קבצי השרת
set(SERVER_SOURCES
    src/server.cpp
    src/game.cpp
)

# צור executable
add_executable(server ${SERVER_SOURCES})

# Include directories
target_include_directories(server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../external
)

if(ixwebsocket_FOUND)
    target_link_libraries(server 
        common
        ixwebsocket::ixwebsocket
    )
else()
    target_include_directories(server PRIVATE ${IXWEBSOCKET_INCLUDE_DIR})
    target_link_directories(server PRIVATE ${IXWEBSOCKET_LIB_DIR})
    target_link_libraries(server 
        common
        ixwebsocket
    )
endif()
