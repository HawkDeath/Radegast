#ifndef RGDEVICE_H
#define RGDEVICE_H

#include <platform/rgWindow.h>

#include <vulkan/vulkan.h>

namespace rg {
    class rgDevice {
    public:
        // input: extensions to enable, reference to window (for surface creation)
        rgDevice(Window &window);

        ~rgDevice();

        VkPhysicalDevice get_physical_device_handle() const { return m_physical_device; }
        VkSurfaceKHR get_surface_handle() const { return m_surface; }
        VkDevice get_device_handle() const { return m_device; }
        VkQueue get_graphics_queue() const { return m_graphcis_queue; }
        VkQueue get_present_queue() const { return m_present_queue; }
        VkQueue get_compute_queue() const { return m_compute_queue; }

    private:
        void create_instance();

        void pick_physical_device();

        void create_device();

    private:
        Window &m_window;
        VkInstance m_instance;
        VkPhysicalDevice m_physical_device;
        VkSurfaceKHR m_surface;
        VkDevice m_device;

        VkPhysicalDeviceProperties m_device_properties;

        VkQueue m_graphcis_queue;
        VkQueue m_present_queue;
        VkQueue m_compute_queue;
    };
}

#endif //RGDEVICE_H
