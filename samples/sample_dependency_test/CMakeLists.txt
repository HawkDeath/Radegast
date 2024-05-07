cmake_minimum_required(VERSION 3.28)
project(0_sample_dependency_test)

add_executable(0_sample_dependency_test main.cpp)
target_link_libraries(0_sample_dependency_test PRIVATE vulkan glm spdlog::spdlog_header_only glfw)