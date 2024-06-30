cmake_minimum_required(VERSION 3.28)
project(RadegastLib)

set(RadegastLib_SOURCE
        src/rendering/core/rgDevice.cpp
        src/platform/rgWindow.cpp )

add_library(RadegastLib STATIC ${RadegastLib_SOURCE})
target_link_libraries(RadegastLib PRIVATE spdlog::spdlog_header_only glm glfw vulkan)
target_include_directories(RadegastLib BEFORE PUBLIC include)