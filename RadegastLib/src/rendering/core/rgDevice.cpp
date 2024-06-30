#include <rendering/core/rgDevice.h>
#include <common/rgLogger.h>
#include <common/rgHelper.h>

#include <set>

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// TODO: add PhysicalDevice abstration to manage a properties and requried extensions

namespace rg {
    rgDevice::rgDevice(Window &window) : m_instance{VK_NULL_HANDLE}, m_window{window}, m_device{VK_NULL_HANDLE} {
        create_instance();
        m_window.create_surface(m_instance, m_surface);
        pick_physical_device();
        create_device();
    }

    rgDevice::~rgDevice() {
        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
        LOGI("rgDevice - Device has been deleted");
    }

    void rgDevice::create_instance() {
        VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = VK_NULL_HANDLE,
            .pApplicationName = "radegest_app",
            .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
            .pEngineName = "radegest_engine",
            .engineVersion = VK_MAKE_VERSION(0, 0, 1),
            .apiVersion = VK_MAKE_API_VERSION(1, 3, 0, 0)
        };

        auto extensiosn = rg::Window::get_required_extensions();
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

    void rgDevice::pick_physical_device() {
        uint32_t device_count = 0u;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

        if (device_count == 0u) {
            RT_THROW("Cannot find ANY GPU supported Vulkan API (Vulkan API 1.3)");
        }
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());
        LOGI("Available GPU device(s): {}", device_count);

        // just print all devices
        for (const auto &device: devices) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);
            LOGI("\t - {}", props.deviceName);
        }
        for (const auto &device: devices) {
            // for now we will looking for dedicated device
            // in future I need to create algorith to choose device base on required extensions
            // thats mean if I will need RT extension then GPU must support this feature
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                m_physical_device = device;
            }
        }

        if (m_physical_device == VK_NULL_HANDLE) {
            m_physical_device = devices[0];
        }

        vkGetPhysicalDeviceProperties(m_physical_device, &m_device_properties);
        LOGI("Chosen GPU: {}", m_device_properties.deviceName);
    }


    void rgDevice::create_device() {
        rg::QueueFamilyIndices indices = findQueueFamilies(m_physical_device, m_surface);

        std::vector<VkDeviceQueueCreateInfo> queues_create_infos;
        std::set<uint32_t> unique_queues_familes = {
            indices.graphcisQueue.value(), indices.presentQueue.value(), indices.computeQueue.value()
        };

        float prioQueue = 1.0f;
        for (uint32_t queueFamily: unique_queues_familes) {
            VkDeviceQueueCreateInfo queue_create_info = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = VK_NULL_HANDLE,
                .flags = (VkDeviceQueueCreateFlags) 0u,
                .queueFamilyIndex = queueFamily,
                .queueCount = 1u,
                .pQueuePriorities = &prioQueue
            };
            queues_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures required_device_features {};
        required_device_features.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo device_create_info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = VK_NULL_HANDLE, // TODO: extensions chain shoule be provide by PhysicalDevice obejct
            .flags = (VkDeviceCreateFlags) 0u,
            .queueCreateInfoCount = static_cast<uint32_t>(queues_create_infos.size()),
            .pQueueCreateInfos = queues_create_infos.data(),
            .enabledLayerCount = 0u,
            .ppEnabledLayerNames = VK_NULL_HANDLE,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &required_device_features

        };

        VK_CHECK(vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device), "Failed to create device");

        vkGetDeviceQueue(m_device, indices.graphcisQueue.value(), 0u, &m_graphcis_queue);
        vkGetDeviceQueue(m_device, indices.presentQueue.value(), 0u, &m_present_queue);
        vkGetDeviceQueue(m_device, indices.computeQueue.value(), 0u, &m_compute_queue);
    }
}
