# cmake_minimum_required(VERSION 3.16)
# project(KungFuChess)

# # Set C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Find all source files in src directory
# file(GLOB_RECURSE SOURCES "src/*.cpp")
# file(GLOB_RECURSE HEADERS "src/*.hpp")

# # Create executable
# add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# # add_executable(physics_tests test_physics.cpp)
# # target_include_directories(physics_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)


# # Set OpenCV paths
# set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/OpenCV_451")
# set(OPENCV_INCLUDE_DIR "${OPENCV_DIR}/include")
# set(OPENCV_LIB_DIR "${OPENCV_DIR}/bin")

# # Set JSON include path
# set(JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# # Include directories
# target_include_directories(${PROJECT_NAME} PRIVATE ${OPENCV_INCLUDE_DIR} ${JSON_INCLUDE_DIR})


# # Link directories
# target_link_directories(${PROJECT_NAME} PRIVATE ${OPENCV_LIB_DIR})
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann)

# # Link OpenCV libraries - use different libraries for Debug vs Release
# target_link_libraries(${PROJECT_NAME} 
#     $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.lib>
#     $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.lib>
#     $<$<CONFIG:RelWithDebInfo>:${OPENCV_LIB_DIR}/opencv_world451.lib>
#     $<$<CONFIG:MinSizeRel>:${OPENCV_LIB_DIR}/opencv_world451.lib>
# )

# # Copy OpenCV DLLs to output directory
# if(WIN32)
#     add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.dll>
#         $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.dll>
#         $<$<CONFIG:RelWithDebInfo>:${OPENCV_LIB_DIR}/opencv_world451.dll>
#         $<$<CONFIG:MinSizeRel>:${OPENCV_LIB_DIR}/opencv_world451.dll>
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>)
# endif()

# # Print found sources for debugging
# message(STATUS "Found source files: ${SOURCES}")
# message(STATUS "Found header files: ${HEADERS}") 


cmake_minimum_required(VERSION 3.10)
project(SFML_Audio_Example)

set(CMAKE_CXX_STANDARD 17)

# הפניה לספריות SFML דרך vcpkg
find_package(SFML 2.5 COMPONENTS audio REQUIRED)

add_executable(audio-test main.cpp)
target_link_libraries(audio-test sfml-audio sfml-system)
