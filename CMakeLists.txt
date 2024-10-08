cmake_minimum_required(VERSION 3.9)
project(rnode
        VERSION 0.1.0
        LANGUAGES C
        DESCRIPTION
        "virtual bus for IPC and inter proc communication")


find_library(NANOMSG_LIBRARIES NAMES nanomsg)
find_package(nanomsg)


set(RNODE_FILES
        rnode.c
        rnode.h
        rnode_sem.c
        rnode_sem.h)

set(RNODE_TEST_FILES
        tests/test.c
        tests/help.c
        tests/help.h
        tests/test.h
        tests/urls.h
        tests/test_node.c
        tests/test_node.h
        tests/test_proc.c
        tests/test_proc.h)

# clang-format
find_program(clang_format NAMES clang-format)
if (NOT clang_format)
    message(STATUS "Could not locate clang-format")
else ()
    file(GLOB format_files ${RNODE_TEST_FILES} ${RNODE_FILES})
    add_custom_target(${PROJECT_NAME}_format COMMAND ${clang_format} -i -fallback-style=none -style=file --verbose ${format_files})
endif ()


add_subdirectory(runit)
add_subdirectory(rlog)
add_subdirectory(rtimeout)
add_library(${PROJECT_NAME} STATIC ${RNODE_FILES})

target_include_directories(${PROJECT_NAME}
        PUBLIC ./
        PRIVATE ${nanomsg_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE nanomsg)


if (NOT ${CMAKE_SYSTEM_NAME} MATCHES Generic)
    add_executable(${PROJECT_NAME}_test ${RNODE_TEST_FILES} ${RNODE_FILES})
    target_link_libraries(${PROJECT_NAME}_test PRIVATE nanomsg rlog rtimeout runit)
endif ()

