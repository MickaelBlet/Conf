cmake_minimum_required(VERSION 2.6)

# set PROJECT_NAME variable
project(configator CXX)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library("${PROJECT_NAME}" SHARED ${SOURCES})
target_compile_options("${PROJECT_NAME}" PRIVATE -pedantic -Wall -Wextra -Werror)

set_target_properties("${PROJECT_NAME}"
    PROPERTIES
        CXX_STANDARD 98
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        NO_SYSTEM_FROM_IMPORTED ON
        PUBLIC_HEADER "include/configator.hpp"
        INTERFACE_INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>;$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
)

if(DEFINED BUILD_TESTING)
    message(STATUS "Enable BUILD_TESTING: ${BUILD_TESTING}")
    enable_testing()
    add_subdirectory(test)
endif()