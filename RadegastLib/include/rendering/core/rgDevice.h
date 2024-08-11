#ifndef RGDEVICE_H
#define RGDEVICE_H

#include <common/rgHelper.h>

#include <platform/rgWindow.h>
#include <rendering/core/rgInstance.h>
#include <rendering/core/rgPhysicalDevice.h>
#include <vulkan/vulkan.h>

// Temporary solution - TODO: create allocator class for memory allocator
#include <vk_mem_alloc.h>

namespace rg {
    class Device {
    public:
        // input: extensions to enable, reference to window (for surface creation)
        Device(Window &window, Instance &instance, const PhysicalDevice& physical_device, ExtensionMap device_extensions = {});

        ~Device();

        PhysicalDevice get_physical_device_handle() const { return m_physical_device; }
        VkSurfaceKHR get_surface_handle() const { return m_surface; }
        VkDevice get_device_handle() const { return m_device; }
        VkQueue get_graphics_queue() const { return m_graphcis_queue; }
        VkQueue get_present_queue() const { return m_present_queue; }
        VkQueue get_compute_queue() const { return m_compute_queue; }

        static VmaAllocator& get_memory_allocator();

    private:
        void create_device(ExtensionMap &device_extensions);

    private:
        Window &m_window;
        Instance &m_instance;
        PhysicalDevice m_physical_device;

        VkSurfaceKHR m_surface;
        VkDevice m_device;

        VkPhysicalDeviceProperties m_device_properties;

        VkQueue m_graphcis_queue;
        VkQueue m_present_queue;
        VkQueue m_compute_queue;
    };
}

#endif //RGDEVICE_H
