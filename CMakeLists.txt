cmake_minimum_required(VERSION 2.6)

# set PROJECT_NAME variable
project(configator CXX)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -pedantic -Wall -Wextra -Werror)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 98
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        PUBLIC_HEADER "include/configator.hpp"
)

# create package
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake\")
set_property(TARGET ${PROJECT_NAME}
             APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES \"\${CMAKE_CURRENT_LIST_DIR}/../../include/${PROJECT_NAME}\")"
)

install(TARGETS ${PROJECT_NAME} EXPORT "${PROJECT_NAME}Targets"
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        PRIVATE_HEADER DESTINATION "include/${PROJECT_NAME}"
        PUBLIC_HEADER DESTINATION "include/${PROJECT_NAME}"
)
install(EXPORT "${PROJECT_NAME}Targets"
        FILE "${PROJECT_NAME}Targets.cmake"
        DESTINATION "lib/cmake"
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION "lib/cmake"
)

if("${BUILD_TESTING}" STREQUAL "ON")
    enable_testing()
    add_subdirectory(test)
endif()