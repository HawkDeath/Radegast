#include <platform/rgWindow.h>
#include <common/Logger.h>
#include <GLFW/glfw3.h>


namespace rg {
    Window::Window() : window_handler{nullptr} {

        glfwSetErrorCallback([](int code, const char *desc) -> void {
            LOGE("[GLFW_error] {}: {}", code, desc);
        });

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window_handler = glfwCreateWindow(1920, 1080, "dummy_test", nullptr, nullptr);
        LOGI("Window has been created");
    }

    Window::~Window() {
        if (window_handler)
            glfwDestroyWindow(window_handler);
        LOGI("Shutting down window");
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