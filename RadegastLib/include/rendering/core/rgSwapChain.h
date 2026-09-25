#ifndef RADEGAST_RGSWAPCHAIN_H
#define RADEGAST_RGSWAPCHAIN_H

#include <rendering/core/rgDevice.h>

#include <vulkan/vulkan_core.h>

#include <vector>

namespace rg {
    class SwapChain final {
    public:
        explicit SwapChain(Device& device, VkExtent2D ext);
        ~SwapChain();

        VkCommandBuffer beginFrame();
        void endFrame(VkCommandBuffer cmd);
        void submit(VkCommandBuffer cmd, bool present = true);

        uint32_t MAX_FRAME_IN_FLIGHT = 2u;
    private:
        void createSwapChain();
        void createSyncObjects();

    private:
        Device& m_device;
        VkSwapchainKHR m_swap_chain_handle {VK_NULL_HANDLE};
        VkExtent2D m_swap_chain_size;

        std::vector<VkImage> m_swap_chain_images;
        std::vector<VkImageView> m_swap_chain_image_views;

        VkImage m_depth_image;
        VkDeviceMemory m_depth_image_memory;
        VkImageView m_depth_image_view;
        VmaAllocation imageAllocation;

        std::vector<VkFence> m_fences;
        std::vector<VkSemaphore> m_present_semaphores;
        std::vector<VkSemaphore> m_render_semaphores;

        std::vector<VkCommandBuffer> m_command_buffers;

        uint32_t m_current_frame{0u};
        uint32_t m_image_idx{0u};

    };
}

#endif //RADEGAST_RGSWAPCHAIN_H
