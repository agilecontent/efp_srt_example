cmake_minimum_required(VERSION 3.10)
project(cppSRTFramingExample)

set(CMAKE_CXX_STANDARD 17)

variable_watch(CMAKE_BUILD_TYPE)

#If no build type is set then force Release
IF( NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
            FORCE)
ENDIF()

#Include EFP
include(ExternalProject)
ExternalProject_Add(project_efp
        GIT_REPOSITORY https://bitbucket.org/unitxtra/efp.git
        GIT_SUBMODULES ""
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/efp
        BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/efp
        GIT_PROGRESS 1
        BUILD_COMMAND cmake --build ${CMAKE_CURRENT_SOURCE_DIR}/efp --config ${CMAKE_BUILD_TYPE} --target efp
        STEP_TARGETS build
        EXCLUDE_FROM_ALL TRUE
        INSTALL_COMMAND ""
        )
add_library(efp STATIC IMPORTED)
set_property(TARGET efp PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/efp/libefp.a)
add_dependencies(efp project_efp)

#Include cppSRTWrapper
ExternalProject_Add(project_cppsrtwrapp
        GIT_REPOSITORY https://github.com/andersc/cppSRTWrapper.git
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/srtwrap
        BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/srtwrap
        GIT_PROGRESS 1
        BUILD_COMMAND cmake --build ${CMAKE_CURRENT_SOURCE_DIR}/srtwrap --config ${CMAKE_BUILD_TYPE} --target srtnet
        STEP_TARGETS build
        EXCLUDE_FROM_ALL TRUE
        INSTALL_COMMAND ""
        )
add_library(srtnet STATIC IMPORTED)
set_property(TARGET srtnet PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/srtwrap/libsrtnet.a)
add_dependencies(srtnet project_cppsrtwrapp)

#Find all required dependencies
find_package (Threads REQUIRED)
find_package(OpenSSL REQUIRED)

#Pass information about where to find headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/srtwrap/srt/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/srtwrap/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/efp/)

#Build the client
add_executable(cppSRTFramingClient client.cpp)
target_link_libraries(cppSRTFramingClient efp srtnet ${CMAKE_CURRENT_SOURCE_DIR}/srtwrap/srt/libsrt.a ${OPENSSL_LIBRARIES} Threads::Threads)

#Build the server
add_executable(cppSRTFramingServer server.cpp)
target_link_libraries(cppSRTFramingServer efp srtnet ${CMAKE_CURRENT_SOURCE_DIR}/srtwrap/srt/libsrt.a ${OPENSSL_LIBRARIES} Threads::Threads)

