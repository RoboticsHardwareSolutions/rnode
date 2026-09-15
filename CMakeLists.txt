cmake_minimum_required(VERSION 3.24)
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


add_library(${PROJECT_NAME} STATIC ${RNODE_FILES})
target_include_directories(${PROJECT_NAME}
        PUBLIC ./
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE ${nanomsg_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE nanomsg)


add_executable(${PROJECT_NAME}_test ${RNODE_TEST_FILES})
if (NOT TARGET runit)
    include(FetchContent)
    FetchContent_Declare(
            runit
            GIT_REPOSITORY https://github.com/RoboticsHardwareSolutions/runit.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(runit)
endif ()
if (NOT TARGET rlog)
    include(FetchContent)
    FetchContent_Declare(
            rlog
            GIT_REPOSITORY https://github.com/RoboticsHardwareSolutions/rlog.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(rlog)
endif ()

if (NOT TARGET rtimeout)
    include(FetchContent)
    FetchContent_Declare(
            rtimeout
            GIT_REPOSITORY https://github.com/RoboticsHardwareSolutions/rtimeout.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(rtimeout)
endif ()
target_link_libraries(${PROJECT_NAME}_test runit rnode rlog rtimeout nanomsg)
enable_testing()
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)



#################### FORMAT SECTION #######################
set(FILES_FOR_FORMATTING ${RNODE_TEST_FILES} ${RNODE_FILES} )

if (NOT TARGET rlibhelper)
    include(FetchContent)
    FetchContent_Declare(
            rlibhelper
            GIT_REPOSITORY https://github.com/RoboticsHardwareSolutions/rlibhelper.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(rlibhelper)
endif ()

if(EXISTS "${rlibhelper_SOURCE_DIR}/format.cmake")
    include(${rlibhelper_SOURCE_DIR}/format.cmake)
else()
    message(WARNING "format.cmake file not found")
endif()
format_files(${PROJECT_NAME} SOURCES ${FILES_FOR_FORMATTING})



