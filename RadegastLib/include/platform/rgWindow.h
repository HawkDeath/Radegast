#ifndef RADEGAST_RGWINDOW_H
#define RADEGAST_RGWINDOW_H

#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

namespace rg {
    class Window {
    public:
        explicit Window(uint32_t width, uint32_t height, const std::string &title);

        ~Window();

        bool should_close() const;

        void create_surface(VkInstance instance, VkSurfaceKHR &surface);

        static std::vector<const char *> get_required_surface_extensions();

        static void update_events();

    private:
        GLFWwindow *m_window_handler;
    };
}

#endif //RADEGAST_RGWINDOW_H
