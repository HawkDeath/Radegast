#ifndef RGPHYSICALDEVICE_H
#define RGPHYSICALDEVICE_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

/*
 * Implementation base on Vulkan-Samples/framework/core/physical_device.cpp
 */

namespace rg {

    class PhysicalDevice {
    public:
        PhysicalDevice(VkPhysicalDevice physical_device_handle);

        ~PhysicalDevice() = default;

        bool is_extension_supported(const std::string &req_extension) const;

        VkBool32 is_present_supported(VkSurfaceKHR surface, uint32_t queue_family_idx) const;

        const VkFormatProperties get_format_properties(VkFormat format) const;

        [[nodiscard]] VkPhysicalDevice get_physical_device_handle() const;

        const VkPhysicalDeviceFeatures get_reqeusted_features() const;

        VkPhysicalDeviceFeatures &get_mutable_reqeusted_features();

        void *get_extensions_feature_chain() const;

    private:
        VkPhysicalDevice m_physicalDevice;
        VkPhysicalDeviceFeatures m_available_device_features{};
        VkPhysicalDeviceProperties m_device_properties{};
        VkPhysicalDeviceMemoryProperties m_device_memory_properties{};
        std::vector<VkExtensionProperties> m_device_extensions;

        VkPhysicalDeviceFeatures m_reqested_device_features{};
        void *m_last_requeted_extension_feature_pnext{};
    };
}

#endif //RGPHYSICALDEVICE_H
