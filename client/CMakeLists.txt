cmake_minimum_required(VERSION 3.16)
project(ChessClient)

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

# קבצי הקוח
set(CLIENT_SOURCES
    src/client.cpp
    src/gameClient.cpp
    src/gameRenderer.cpp
    src/mockImg.cpp
    src/soundPlayer.cpp
    src/scoreBoard.cpp
    src/moveList.cpp
    src/background.cpp
)

# צור executable
add_executable(client ${CLIENT_SOURCES})

# Include directories
target_include_directories(client PRIVATE 
    ${OPENCV_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/nlohmann
    ${IXWEBSOCKET_INCLUDE_DIR}
)

# Link directories
target_link_directories(client PRIVATE 
    ${OPENCV_LIB_DIR}
    ${IXWEBSOCKET_LIB_DIR}
)

# קישור לכל הספריות הנדרשות
target_link_libraries(client 
    common
    $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.lib>
    $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.lib>
    ixwebsocket
    crypt32
    bcrypt
    ws2_32
    winmm
)

# העתק OpenCV DLLs
if(WIN32)
    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.dll>
        $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.dll>
        $<TARGET_FILE_DIR:client>)
endif()
