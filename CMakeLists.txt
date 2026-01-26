cmake_minimum_required(VERSION 3.12)
project(GEData)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

message(STATUS "[${PROJECT_NAME}] current cmake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
message(STATUS "[${PROJECT_NAME}] CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM} ")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build Type" FORCE)
endif()

message(STATUS "[${PROJECT_NAME}] Build type:${CMAKE_BUILD_TYPE}")

message(STATUS "[${PROJECT_NAME}] Debug configuration:${CMAKE_CXX_FLAGS_DEBUG}")

message(STATUS "[${PROJECT_NAME}] release configuration:${CMAKE_CXX_FLAGS_RELEASE}")

message(STATUS "[${PROJECT_NAME}] release configuration with debug info:${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

message(STATUS "[${PROJECT_NAME}] minimal release configuration:${CMAKE_CXX_FLAGS_MINSIZEREL}")

include_directories(include)


set(GE_LIB_NAME gedata)
add_library(${GE_LIB_NAME}
    src/GEAnalytics.cpp
    src/GEUtils.cpp
    src/GEBatchConsumer.cpp
    src/GEDebugConsumer.cpp
    src/GENetwork.cpp
    src/GEConsumer.cpp  # 确保这个文件被包含
)

if(WIN32)
    include_directories(thirdParty/curl/include)
    link_directories(thirdParty/curl/lib)
    target_link_libraries(${GE_LIB_NAME} libcurl)
else()
    include_directories(thirdParty/curl/include)
    find_package(Threads REQUIRED)
    target_link_libraries(${GE_LIB_NAME} curl Threads::Threads)
endif()

add_executable(demo example/demo.cpp)
target_link_libraries(demo ${GE_LIB_NAME})
