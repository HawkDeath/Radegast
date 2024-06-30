#include <common/rgHelper.h>
#include <vector>


namespace rg {
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        QueueFamilyIndices indices;

        uint32_t queueFamilesCount = 0u;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilesCount, nullptr);
        std::pmr::vector<VkQueueFamilyProperties> queue_family_propertieses(queueFamilesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilesCount, queue_family_propertieses.data());

        uint32_t i = 0;
        for (const auto &queueProp: queue_family_propertieses) {
            if (queueProp.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphcisQueue = i;
            }
            if (queueProp.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.computeQueue = i;
            }

            VkBool32 presentSupported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupported);

            if (presentSupported) {
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
