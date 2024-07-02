#include <rendering/core/rgDevice.h>
#include <common/rgLogger.h>
#include <common/rgHelper.h>

#include <set>

namespace rg {
    // add two paramters for instance extensions and device extensions, add PhysicalDevice paramter
    Device::Device(Window &window, Instance &instance, PhysicalDevice physical_device,
                   ExtensionMap device_extensions) : m_instance{instance}, m_window{window},
                                                     m_physical_device{physical_device},
                                                     m_device{VK_NULL_HANDLE} {
        m_window.create_surface(m_instance.get_instance_handler(), m_surface);
        create_device(device_extensions);
    }

    Device::~Device() {
        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance.get_instance_handler(), m_surface, nullptr);
        LOGI("rgDevice - Device has been deleted");
    }


    void Device::create_device(ExtensionMap &device_extensions) {
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

        VkPhysicalDeviceFeatures required_device_features{};
        required_device_features.samplerAnisotropy = VK_TRUE;
        std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        for (auto &ext: device_extensions) {
            if (ext.second == false) {
                // meaning extension is optional
                deviceExtensions.push_back(ext.first);
            }
        }


        VkDeviceCreateInfo device_create_info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = m_physical_device.get_extensions_feature_chain(),
            .flags = (VkDeviceCreateFlags) 0u,
            .queueCreateInfoCount = static_cast<uint32_t>(queues_create_infos.size()),
            .pQueueCreateInfos = queues_create_infos.data(),
            .enabledLayerCount = 0u,
            .ppEnabledLayerNames = VK_NULL_HANDLE,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &m_physical_device.get_mutable_reqeusted_features()

        };

        VK_CHECK(
            vkCreateDevice(m_physical_device.get_physical_device_handle(), &device_create_info, nullptr, &m_device),
            "Failed to create device");

        vkGetDeviceQueue(m_device, indices.graphcisQueue.value(), 0u, &m_graphcis_queue);
        vkGetDeviceQueue(m_device, indices.presentQueue.value(), 0u, &m_present_queue);
        vkGetDeviceQueue(m_device, indices.computeQueue.value(), 0u, &m_compute_queue);
    }
}
