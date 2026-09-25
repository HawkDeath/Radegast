#include <rendering/core/rgDevice.h>
#include <common/rgLogger.h>
#include <common/rgHelper.h>

#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1003000 // Vulkan 1.3
#include <vk_mem_alloc.h>

#include <set>

namespace rg {
    // add two parameters for instance extensions and device extensions, add PhysicalDevice parameter
    Device::Device(Window &window, Instance &instance, const PhysicalDevice& physical_device,
                   ExtensionMap device_extensions) : m_instance{instance}, m_window{window},
                                                     m_physical_device{physical_device},
                                                     m_device{VK_NULL_HANDLE} {
        LOGI("Chosen GPU: {}", m_physical_device.get_physical_device_properties().deviceName);
        m_window.create_surface(m_instance.get_instance_handler(), m_surface);
        create_device(device_extensions);


        VkCommandPoolCreateInfo cmd_pool_ci {};
        cmd_pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmd_pool_ci.pNext = VK_NULL_HANDLE;
        cmd_pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmd_pool_ci.queueFamilyIndex = m_graphcis_queue.idx;

        VK_CHECK(vkCreateCommandPool(m_device, &cmd_pool_ci, nullptr, &m_command_pool), "Failed to create command pool");

        // bind VMA
        VmaVulkanFunctions vma_vulkan_functions {};
        vma_vulkan_functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vma_vulkan_functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo vma_allocator_create_info {};
        vma_allocator_create_info.flags = 0u; // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT; // TODO: make flag creation more flexible
        vma_allocator_create_info.pVulkanFunctions = &vma_vulkan_functions;
        vma_allocator_create_info.instance = m_instance.get_instance_handler();
        vma_allocator_create_info.physicalDevice = m_physical_device.get_physical_device_handle();
        vma_allocator_create_info.device = m_device;

        VK_CHECK(vmaCreateAllocator(&vma_allocator_create_info, &get_memory_allocator()), "Failed to create VMA allocator");
    }

    Device::~Device() {
        vkDestroyCommandPool(m_device, m_command_pool, nullptr);
        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance.get_instance_handler(), m_surface, nullptr);
        auto &alloc = get_memory_allocator();
        if (alloc != VK_NULL_HANDLE)
        {
            VmaTotalStatistics stats;
            vmaCalculateStatistics(alloc, &stats);
            LOGI("Total device memory leaked: {} bytes.", stats.total.statistics.allocationBytes);
            vmaDestroyAllocator(alloc);
            alloc = VK_NULL_HANDLE;
        }

        LOGI("rgDevice - Device has been deleted");
    }
    VmaAllocator& Device::get_memory_allocator()
    {
        static VmaAllocator global_memory_allocator = VK_NULL_HANDLE; // allocator must be create by vmaCreateAllocator function
        return global_memory_allocator;
    }


    void Device::create_device(ExtensionMap &device_extensions) {
        QueueFamilyIndices indices = findQueueFamilies(m_physical_device, m_surface);

        std::vector<VkDeviceQueueCreateInfo> queues_create_infos;
        std::set unique_queues_familes = {
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
        m_graphcis_queue.idx = indices.graphcisQueue.value();
        m_present_queue.idx = indices.presentQueue.value();
        m_compute_queue.idx = indices.computeQueue.value();

        vkGetDeviceQueue(m_device, m_graphcis_queue.idx, 0u, &m_graphcis_queue.handle);
        vkGetDeviceQueue(m_device, m_present_queue.idx, 0u, &m_present_queue.handle);
        vkGetDeviceQueue(m_device, m_compute_queue.idx, 0u, &m_compute_queue.handle);
    }
}
