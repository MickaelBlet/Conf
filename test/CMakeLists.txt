cmake_minimum_required(VERSION 2.6)

if(NOT "${PROJECT_NAME}" STREQUAL "configator")
    enable_testing()
    project(configator_test CXX)
    find_package(configator REQUIRED)
else()
    project(configator_test CXX)
endif()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

file(GLOB test_files "*.cpp")

foreach(file ${test_files})
    get_filename_component(filenamewe ${file} NAME_WE)
    add_executable("configator_${filenamewe}.gtest" ${file})
    set_target_properties("configator_${filenamewe}.gtest"
        PROPERTIES
            CXX_STANDARD 98
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )
    target_compile_options("configator_${filenamewe}.gtest" PRIVATE -pedantic -Wall -Wextra -Werror)
    target_link_libraries("configator_${filenamewe}.gtest" gtest_main gmock pthread)
    target_link_libraries("configator_${filenamewe}.gtest" configator)
    add_test(NAME "configator_${filenamewe}.gtest" COMMAND "configator_${filenamewe}.gtest")
endforeach()
