#ifndef RGPHYSICALDEVICE_H
#define RGPHYSICALDEVICE_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace rg {

    class PhysicalDevice final {
    public:
        PhysicalDevice() = default;
        PhysicalDevice(VkPhysicalDevice physical_device_handle);

        ~PhysicalDevice() = default;

        bool is_extension_supported(const std::string &req_extension) const;

        VkBool32 is_present_supported(VkSurfaceKHR surface, uint32_t queue_family_idx) const;

        const VkFormatProperties get_format_properties(VkFormat format) const;

        [[nodiscard]] VkPhysicalDevice get_physical_device_handle() const;

        const VkPhysicalDeviceFeatures get_reqeusted_features() const;

        VkPhysicalDeviceFeatures &get_mutable_reqeusted_features();

        VkPhysicalDeviceProperties get_physical_device_properties() const;

        void *get_extensions_feature_chain() const;

    private:
        VkPhysicalDevice m_physicalDevice {VK_NULL_HANDLE};
        VkPhysicalDeviceFeatures m_available_device_features{};
        VkPhysicalDeviceProperties m_device_properties{};
        VkPhysicalDeviceMemoryProperties m_device_memory_properties{};
        std::vector<VkExtensionProperties> m_device_extensions;

        VkPhysicalDeviceFeatures m_reqested_device_features{};
        void *m_last_requeted_extension_feature_pnext{nullptr};
    };
}

#endif //RGPHYSICALDEVICE_H
