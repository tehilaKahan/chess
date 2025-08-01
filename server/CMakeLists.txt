cmake_minimum_required(VERSION 3.16)
project(ChessServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# תיקון runtime library mismatch
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

# הגדר OpenCV paths
set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../OpenCV_451")
set(OPENCV_INCLUDE_DIR "${OPENCV_DIR}/include")
set(OPENCV_LIB_DIR "${OPENCV_DIR}/bin")

# נסה למצוא ixwebsocket
find_package(ixwebsocket CONFIG QUIET)
if(NOT ixwebsocket_FOUND)
    set(IXWEBSOCKET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/installed/x64-windows/include")
    set(IXWEBSOCKET_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/installed/x64-windows/lib")
endif()

# קבצי השרת
set(SERVER_SOURCES
    src/server.cpp
    src/game.cpp
)

add_executable(server ${SERVER_SOURCES})

target_include_directories(server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/../client/src
    ${OPENCV_INCLUDE_DIR}
    ${IXWEBSOCKET_INCLUDE_DIR}
)

target_link_directories(server PRIVATE 
    ${OPENCV_LIB_DIR}
    ${IXWEBSOCKET_LIB_DIR}
)

target_link_libraries(server 
    common
    $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.lib>
    $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.lib>
    ixwebsocket
    crypt32
    bcrypt
    ws2_32
    winmm
)
