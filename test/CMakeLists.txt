include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

file(GLOB test_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(file ${test_files})
    get_filename_component(filenamewe ${file} NAME_WE)
    add_executable("configator_${filenamewe}.gtest" ${file})
    set_target_properties("configator_${filenamewe}.gtest"
        PROPERTIES
            CXX_STANDARD 98
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            NO_SYSTEM_FROM_IMPORTED ON
    )
    target_compile_options("configator_${filenamewe}.gtest" PRIVATE -pedantic -Wall -Wextra -Werror)
    target_link_libraries("configator_${filenamewe}.gtest" gtest_main gtest pthread)
    target_link_libraries("configator_${filenamewe}.gtest" configator)
    add_test(NAME "configator_${filenamewe}.gtest" COMMAND "configator_${filenamewe}.gtest")
endforeach()

if (BUILD_COVERAGE)
    add_test(NAME "gcov_configator.cpp" COMMAND sh -c "find \"${CMAKE_CURRENT_BINARY_DIR}/..\" -name \"configator.cpp.gcda\" -exec sh -c \"gcov -n \\\"{}\\\" | grep -A 1 \\\"configator.cpp\\\" | grep \\\":\\\" | sed 's/[^:]\\+[:]\\([0-9]\\+[.][0-9]\\+%\\).*/\\1/g'\" \\;")
    set_property(TEST "gcov_configator.cpp" PROPERTY PASS_REGULAR_EXPRESSION "^100.00%")
    add_test(NAME "gcov_configator_map.cpp" COMMAND sh -c "find \"${CMAKE_CURRENT_BINARY_DIR}/..\" -name \"configator_map.cpp.gcda\" -exec sh -c \"gcov -n \\\"{}\\\" | grep -A 1 \\\"configator_map.cpp\\\" | grep \\\":\\\" | sed 's/[^:]\\+[:]\\([0-9]\\+[.][0-9]\\+%\\).*/\\1/g'\" \\;")
    set_property(TEST "gcov_configator_map.cpp" PROPERTY PASS_REGULAR_EXPRESSION "^100.00%")
endif() # BUILD_COVERAGE