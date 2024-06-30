#ifndef RGHELPER_H
#define RGHELPER_H
#include <optional>
#include <cstdint>

#include <vulkan/vulkan.h>

namespace rg {
struct QueueFamilyIndices {
    std::optional<uint32_t> graphcisQueue;
    std::optional<uint32_t> presentQueue;
    std::optional<uint32_t> computeQueue;

    [[nodiscard]] bool isComplete() const {
        return graphcisQueue.has_value() && presentQueue.has_value() && computeQueue.has_value();
    }
};

 QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);



}
#endif //RGHELPER_H
