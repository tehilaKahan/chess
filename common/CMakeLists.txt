cmake_minimum_required(VERSION 3.16)
project(ChessCommon)

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

# מצא את כל קבצי המקור
file(GLOB_RECURSE COMMON_SOURCES "src/*.cpp")
file(GLOB_RECURSE COMMON_HEADERS "src/*.hpp")

# צור ספרייה סטטית
add_library(common STATIC ${COMMON_SOURCES} ${COMMON_HEADERS})

# Include directories
target_include_directories(common PUBLIC 
    src
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/nlohmann
    ${OPENCV_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../client/src
)

# Link directories
target_link_directories(common PRIVATE ${OPENCV_LIB_DIR})

# קישור ל-OpenCV
target_link_libraries(common 
    $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.lib>
    $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.lib>
)

message(STATUS "Common sources: ${COMMON_SOURCES}")
