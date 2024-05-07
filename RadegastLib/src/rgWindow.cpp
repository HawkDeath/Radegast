#include <rgWindow.h>
#include <spdlog/spdlog.h>

namespace rg {
    Window::Window() : window_handler{nullptr} {

        glfwSetErrorCallback([](int code, const char *desc) -> void {
            spdlog::error("[GLFW_error] {}: {}", code, desc);
        });

        glfwInit();

        window_handler = glfwCreateWindow(1920, 1080, "dummy_test", nullptr, nullptr);
        spdlog::info("Window has been created");
    }

    Window::~Window() {
        if (window_handler)
            glfwDestroyWindow(window_handler);
        spdlog::info("Shutting down window");
        glfwTerminate();
    }

    bool Window::should_close() {
        return glfwWindowShouldClose(window_handler);
    }

    void Window::update_events()
    {
        glfwPollEvents();
    }
}