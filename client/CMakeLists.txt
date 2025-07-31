cmake_minimum_required(VERSION 3.16)
project(ChessClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# נסה למצוא ספריות, אם לא - השתמש בנתיבים ידניים
find_package(ixwebsocket CONFIG QUIET)
find_package(SFML 2.5 COMPONENTS audio QUIET)

# הגדר OpenCV paths
set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../OpenCV_451")
set(OPENCV_INCLUDE_DIR "${OPENCV_DIR}/include")
set(OPENCV_LIB_DIR "${OPENCV_DIR}/bin")

# קבצי הקוח
set(CLIENT_SOURCES
    src/client.cpp
    src/gameClient.cpp
    src/gameRenderer.cpp
    src/graphics.cpp
    src/img.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../external
)

# Link directories
target_link_directories(client PRIVATE ${OPENCV_LIB_DIR})

# קישור לספריות בסיסיות
target_link_libraries(client 
    common
    $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.lib>
    $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.lib>
)

# הוסף ixwebsocket אם נמצא
if(ixwebsocket_FOUND)
    target_link_libraries(client ixwebsocket::ixwebsocket)
endif()

# הוסף SFML אם נמצא
if(SFML_FOUND)
    target_link_libraries(client sfml-audio sfml-system)
endif()

# העתק OpenCV DLLs
if(WIN32)
    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.dll>
        $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.dll>
        $<TARGET_FILE_DIR:client>)
endif()
