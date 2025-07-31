cmake_minimum_required(VERSION 3.16)
project(ChessCommon)

# הגדר OpenCV paths
set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../OpenCV_451")
set(OPENCV_INCLUDE_DIR "${OPENCV_DIR}/include")

# מצא את כל קבצי המקור
file(GLOB_RECURSE COMMON_SOURCES "src/*.cpp")
file(GLOB_RECURSE COMMON_HEADERS "src/*.hpp")

# צור ספרייה סטטית
add_library(common STATIC ${COMMON_SOURCES} ${COMMON_HEADERS})

# Include directories
target_include_directories(common PUBLIC 
    src
    ${CMAKE_CURRENT_SOURCE_DIR}/../external
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/nlohmann  # הוסף נתיב ל-json.hpp
    ${OPENCV_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../client/src
)

# הדפס קבצים שנמצאו
message(STATUS "Common sources: ${COMMON_SOURCES}")
message(STATUS "Common headers: ${COMMON_HEADERS}")
