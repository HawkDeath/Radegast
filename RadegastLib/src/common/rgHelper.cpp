#include <common/rgHelper.h>
#include <rendering/core/rgPhysicalDevice.h>
#include <vector>


namespace rg {
    QueueFamilyIndices findQueueFamilies(const PhysicalDevice &physicalDevice, VkSurfaceKHR surface) {
        QueueFamilyIndices indices;

        uint32_t queueFamilesCount = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.get_physical_device_handle(), &queueFamilesCount,
                                                 nullptr);
        std::pmr::vector<VkQueueFamilyProperties> queue_family_propertieses(queueFamilesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.get_physical_device_handle(), &queueFamilesCount,
                                                 queue_family_propertieses.data());

        uint32_t i = 0;
        for (const auto &queueProp: queue_family_propertieses) {
            if (queueProp.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphcisQueue = i;
            }
            if (queueProp.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.computeQueue = i;
            }

            if (physicalDevice.is_present_supported(surface, i)) {
                indices.presentQueue = i;
            }

            if (indices.isComplete()) {
                break;
            }
            ++i;
        }

        return indices;
    }
}
