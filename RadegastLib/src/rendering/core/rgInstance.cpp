#include <rendering/core/rgInstance.h>
#include <platform/rgWindow.h>
#include <common/rgLogger.h>

namespace rg {

    Instance::Instance(ExtensionMap instance_extensions ) {
        create_instance(instance_extensions);
    }

    Instance::~Instance() {
        vkDestroyInstance(m_instance, nullptr);
    }


    void Instance::create_instance(ExtensionMap &instance_extensions) {
        VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = VK_NULL_HANDLE,
            .pApplicationName = "radegest_app",
            .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
            .pEngineName = "radegest_engine",
            .engineVersion = VK_MAKE_VERSION(0, 0, 1),
            .apiVersion = VK_MAKE_API_VERSION(1, 3, 0, 0)
        };

        // rg::Window::get_required_surface_extensions() return only extensions required by GLFW
        auto extensiosn = rg::Window::get_required_surface_extensions();

        for (auto &instance_ext : instance_extensions) {
            if (instance_ext.second == false) { // meaning extension is optional
                extensiosn.push_back(instance_ext.first);
            }
        }

        VkInstanceCreateInfo instance_create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE,
            .flags = (VkInstanceCreateFlags) 0U,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = 0u,
            .ppEnabledLayerNames = NULL,
            .enabledExtensionCount = static_cast<uint32_t>(extensiosn.size()),
            .ppEnabledExtensionNames = extensiosn.data()
        };


        VK_CHECK(vkCreateInstance(&instance_create_info, nullptr, &m_instance), "Failed to create instance");
        LOGI("rgDevice - Vulkan Instance has been created");
    }

    std::vector<VkPhysicalDevice> Instance::get_available_physical_device_list() {
        uint32_t device_count = 0u;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

        if (device_count == 0u) {
            RT_THROW("Cannot find ANY GPU supported Vulkan API (Vulkan API 1.3)");
        }
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());
        LOGI("Available GPU device(s): {}", device_count);

        return devices;
    }


}