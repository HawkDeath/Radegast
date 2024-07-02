#ifndef RGHELPER_H
#define RGHELPER_H

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>
#include <map>


namespace rg {
    class PhysicalDevice;
    using ExtensionMap = std::map<const char *, bool>;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphcisQueue;
        std::optional<uint32_t> presentQueue;
        std::optional<uint32_t> computeQueue;

        [[nodiscard]] bool isComplete() const {
            return graphcisQueue.has_value() && presentQueue.has_value() && computeQueue.has_value();
        }
    };

    QueueFamilyIndices findQueueFamilies(const PhysicalDevice &physicalDevice, VkSurfaceKHR surface);
}
#endif //RGHELPER_H
