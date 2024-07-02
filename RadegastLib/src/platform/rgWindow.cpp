#include <platform/rgWindow.h>
#include <common/rgLogger.h>
#include <GLFW/glfw3.h>


namespace rg {
    Window::Window(uint32_t width, uint32_t height, const std::string &title) : m_window_handler{nullptr} {
        glfwSetErrorCallback([](int code, const char *desc) -> void {
            LOGE("[GLFW_error] {}: {}", code, desc);
        });

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window_handler = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr,
                                          nullptr);
        LOGI("Window has been created");

        glfwSetWindowUserPointer(m_window_handler, this);
        // TODO:
        // * resizing

    }

    Window::~Window() {
        if (m_window_handler)
            glfwDestroyWindow(m_window_handler);
        LOGI("Shutting down window");
        glfwTerminate();
    }

    void Window::create_surface(VkInstance instance, VkSurfaceKHR &surface) {
        VK_CHECK(glfwCreateWindowSurface(instance, m_window_handler, nullptr, &surface), "Failed to create VkSurfaceKHR");
    }


    std::vector<const char *> Window::get_required_surface_extensions() {
        uint32_t glfwExtensionsCount = 0u;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
        LOGI("Extensions: ");
        for (auto & e : extensions) {

            LOGI("- {}", e);
        }
        return extensions;
    }


    bool Window::should_close() const {
        return glfwWindowShouldClose(m_window_handler);
    }

    void Window::update_events() {
        glfwPollEvents();
    }
}
