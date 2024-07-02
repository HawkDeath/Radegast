#include <rendering/core/rgPhysicalDevice.h>
#include <common/rgLogger.h>
#include <algorithm>

namespace rg {
    PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device_handle) : m_physicalDevice{physical_device_handle} {
        vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_available_device_features);
        vkGetPhysicalDeviceProperties(m_physicalDevice, &m_device_properties);
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_device_memory_properties);

        LOGI("Found GPU: {}", m_device_properties.deviceName);

        uint32_t device_extension_count = 0u;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &device_extension_count, nullptr),
                 "Failed to get vkEnumerateDeviceExtensionProperties.");
        m_device_extensions = std::vector<VkExtensionProperties>(device_extension_count);
        VK_CHECK(
            vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &device_extension_count, m_device_extensions
                .data()),
            "Failed to get vkEnumerateDeviceExtensionProperties.");

        if (m_device_extensions.empty()) {
            LOGE("Device \'{}\' has not extensions!!!!!!!!!!", m_device_properties.deviceName);
        }
    }

    bool PhysicalDevice::is_extension_supported(const std::string &req_extension) const {
        return std::find_if(m_device_extensions.begin(), m_device_extensions.end(), [req_extension](auto &device_ext) {
            return std::strcmp(device_ext.extensionName, req_extension.c_str()) == 0;
        }) != m_device_extensions.end();
    }

    VkBool32 PhysicalDevice::is_present_supported(VkSurfaceKHR surface, uint32_t queue_family_idx) const {
        VkBool32 present_supported = VK_FALSE;
        if (surface != VK_NULL_HANDLE) {
            VK_CHECK(
                vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, queue_family_idx, surface, &present_supported),
                "Failed to vkGetPhysicalDeviceSurfaceSupportKHR");
        }
        return present_supported;
    }

    const VkFormatProperties PhysicalDevice::get_format_properties(VkFormat format) const {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &format_properties);
        return format_properties;
    }


    void *PhysicalDevice::get_extensions_feature_chain() const {
        return m_last_requeted_extension_feature_pnext;
    }

    [[nodiscard]] VkPhysicalDevice PhysicalDevice::get_physical_device_handle() const { return m_physicalDevice; }

    const VkPhysicalDeviceFeatures PhysicalDevice::get_reqeusted_features() const {
        return m_reqested_device_features;
    }

    VkPhysicalDeviceFeatures &PhysicalDevice::get_mutable_reqeusted_features() { return m_reqested_device_features; }
}
