#include <array>
#include <cstring>
#include <rendering/core/rgSwapChain.h>

namespace rg {
    SwapChain::SwapChain(Device& device, VkExtent2D ext) : m_device{device}, m_swap_chain_size{ext} {
        createSwapChain();
        createSyncObjects();

        m_command_buffers.resize(MAX_FRAME_IN_FLIGHT);
        VkCommandBufferAllocateInfo cmdAllocCI{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_device.get_command_pool_handle(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = MAX_FRAME_IN_FLIGHT};
        vkAllocateCommandBuffers(m_device.get_device_handle(), &cmdAllocCI,
                                 m_command_buffers.data());
    }

    SwapChain::~SwapChain() {
        auto vkDeviceHandle = m_device.get_device_handle();

        vkDeviceWaitIdle(vkDeviceHandle);

        for (auto i = 0u; i < MAX_FRAME_IN_FLIGHT; ++i) {
            vkDestroySemaphore(vkDeviceHandle, m_present_semaphores[i], nullptr);
            vkDestroyFence(vkDeviceHandle, m_fences[i], nullptr);
        }

        for (auto &sem : m_render_semaphores) {
            vkDestroySemaphore(vkDeviceHandle, sem, nullptr);
        }

       vkDestroyImageView(vkDeviceHandle, m_depth_image_view, nullptr);

        for (auto &iv : m_swap_chain_image_views) {
            vkDestroyImageView(vkDeviceHandle, iv, nullptr);
        }
        // destroy depth image

        m_swap_chain_image_views.clear();
        m_swap_chain_images.clear();
        m_present_semaphores.clear();
        m_fences.clear();
        m_render_semaphores.clear();

        if (m_swap_chain_handle) {
            vkDestroySwapchainKHR(vkDeviceHandle, m_swap_chain_handle, nullptr);
        }
    }

    VkCommandBuffer SwapChain::beginFrame() {
        vkWaitForFences(m_device.get_device_handle(), 1u, &m_fences[m_current_frame], true,
                          UINT64_MAX);
          vkResetFences(m_device.get_device_handle(), 1u, &m_fences[m_current_frame]);
          vkAcquireNextImageKHR(m_device.get_device_handle(), m_swap_chain_handle, UINT64_MAX,
                                m_present_semaphores[m_current_frame], VK_NULL_HANDLE,
                                &m_image_idx);

          VkCommandBuffer cmd = m_command_buffers[m_current_frame];

          vkResetCommandBuffer(cmd, 0u);
          VkCommandBufferBeginInfo cmdBegInfo{
              .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
              .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
          vkBeginCommandBuffer(cmd, &cmdBegInfo);

          std::array<VkImageMemoryBarrier2, 2> barriers{
              VkImageMemoryBarrier2{
                  .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                  .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                  .srcAccessMask = 0u,
                  .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                  .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                   VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                  .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                  .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                  .image = m_swap_chain_images[m_image_idx],
                  .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                    .levelCount = 1u,
                                    .layerCount = 1u}

              },
              VkImageMemoryBarrier2{
                  .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                  .srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                  .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                  .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
                  .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                  .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                  .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                  .image = m_depth_image,
                  .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                                    .levelCount = 1u,
                                    .layerCount = 1u}

              }

          };

          VkDependencyInfo depsInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                    .imageMemoryBarrierCount =
                                        static_cast<uint32_t>(barriers.size()),
                                    .pImageMemoryBarriers = barriers.data()};
          vkCmdPipelineBarrier2(cmd, &depsInfo);

          VkRenderingAttachmentInfo colorAttachInfo{
              .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
              .imageView = m_swap_chain_image_views[m_image_idx],
              .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
              .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
              .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
              .clearValue{.color{0.125f, 0.125f, 0.125f, 1.0f}}};

          VkRenderingAttachmentInfo depthAttachInfo{
              .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
              .imageView = m_depth_image_view,
              .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
              .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
              .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
              .clearValue{.depthStencil{1.0f, 0u}}};

          VkRenderingInfo renderInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
                                     .renderArea{.extent = m_swap_chain_size},
                                     .layerCount = 1u,
                                     .colorAttachmentCount = 1u,
                                     .pColorAttachments = &colorAttachInfo,
                                     .pDepthAttachment = &depthAttachInfo};

          vkCmdBeginRendering(cmd, &renderInfo);

          VkViewport viewport{.width = (float)m_swap_chain_size.width,
                              .height = (float)m_swap_chain_size.height,
                              .minDepth = 0.0f,
                              .maxDepth = 1.0f};
          VkRect2D scissors{.extent = m_swap_chain_size};
          vkCmdSetViewport(cmd, 0u, 1u, &viewport);
          vkCmdSetScissor(cmd, 0u, 1u, &scissors);

          return cmd;
    }

    void SwapChain::endFrame(VkCommandBuffer cmd) {

        vkCmdEndRendering(cmd);

        VkImageMemoryBarrier2 barrierPresent{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = 0u,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = m_swap_chain_images[m_current_frame],
            .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                              .levelCount = 1u,
                              .layerCount = 1u}};

        VkDependencyInfo depsInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                  .imageMemoryBarrierCount = 1u,
                                  .pImageMemoryBarriers = &barrierPresent};
        vkCmdPipelineBarrier2(cmd, &depsInfo);
        vkEndCommandBuffer(cmd);
    }

    void SwapChain::submit(VkCommandBuffer cmd, bool present) {
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .waitSemaphoreCount = 1u,
                                .pWaitSemaphores =
                                    &m_present_semaphores[m_current_frame],
                                .pWaitDstStageMask = waitStages,
                                .commandBufferCount = 1u,
                                .pCommandBuffers = &cmd,
                                .signalSemaphoreCount = 1u,
                                .pSignalSemaphores = &m_render_semaphores[m_image_idx]};
        vkQueueSubmit(m_device.get_graphics_queue().handle, 1u, &submitInfo,
                      m_fences[m_current_frame]);

        m_current_frame = (m_current_frame + 1) % MAX_FRAME_IN_FLIGHT;

        if (present) {
            VkPresentInfoKHR presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                         .waitSemaphoreCount = 1u,
                                         .pWaitSemaphores =
                                             &m_render_semaphores[m_image_idx],
                                         .swapchainCount = 1u,
                                         .pSwapchains = &m_swap_chain_handle,
                                         .pImageIndices = &m_image_idx};

            vkQueuePresentKHR(m_device.get_present_queue().handle, &presentInfo);
        }
    }

    void SwapChain::createSwapChain() {
        const VkFormat imageFormat{VK_FORMAT_B8G8R8A8_SRGB};
          VkSurfaceCapabilitiesKHR surfCaps{};
          vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device.get_physical_device_handle().get_physical_device_handle(),
                                                    m_device.get_surface_handle(), &surfCaps);

          VkSwapchainCreateInfoKHR swapchainCI{
              .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
              .surface = m_device.get_surface_handle(),
              .minImageCount = surfCaps.minImageCount,
              .imageFormat = imageFormat,
              .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
              .imageExtent = m_swap_chain_size,
              .imageArrayLayers = 1u,
              .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
              .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
              .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
              .presentMode = VK_PRESENT_MODE_FIFO_KHR

          };

          auto res = vkCreateSwapchainKHR(m_device.get_device_handle(), &swapchainCI, nullptr,
                                          &m_swap_chain_handle);
          if (res != VK_SUCCESS) {
            std::printf("Failed to create swapchain");
          }
          uint32_t imageCount{0u};
          vkGetSwapchainImagesKHR(m_device.get_device_handle(), m_swap_chain_handle, &imageCount, nullptr);
          MAX_FRAME_IN_FLIGHT = std::min(imageCount, MAX_FRAME_IN_FLIGHT); // AMD iGPU fix
          m_swap_chain_images.resize(imageCount);
          m_swap_chain_image_views.resize(imageCount);
          vkGetSwapchainImagesKHR(m_device.get_device_handle(), m_swap_chain_handle, &imageCount,
                                  m_swap_chain_images.data());

          for (size_t i = 0u; i < imageCount; ++i) {
            VkImageViewCreateInfo ivCI{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = m_swap_chain_images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = imageFormat,
                .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .levelCount = 1u,
                                  .layerCount = 1u}};
            vkCreateImageView(m_device.get_device_handle(), &ivCI, nullptr,
                              &m_swap_chain_image_views[i]);

            // depth buffer

            std::vector<VkFormat> depthFormatsCandidates{VK_FORMAT_D32_SFLOAT_S8_UINT,
                                                         VK_FORMAT_D24_UNORM_S8_UINT};
            VkFormat depthFormat{VK_FORMAT_UNDEFINED};

            for (auto &format : depthFormatsCandidates) {
              VkFormatProperties2 formatProps2{
                  .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
              vkGetPhysicalDeviceFormatProperties2(m_device.get_physical_device_handle().get_physical_device_handle(),
                                                    format,
                                                   &formatProps2);
              if (formatProps2.formatProperties.optimalTilingFeatures &
                  VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                depthFormat = format;
                break;
              }
            }

            VkImageCreateInfo depthImageCI{.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                        .imageType = VK_IMAGE_TYPE_2D,
                                        .format = depthFormat,
                                        .extent{.width = m_swap_chain_size.width,
                                                .height = m_swap_chain_size.height,
                                                .depth = 1u},
                                        .mipLevels = 1u,
                                        .arrayLayers = 1u,
                                        .samples = VK_SAMPLE_COUNT_1_BIT,
                                        .tiling = VK_IMAGE_TILING_OPTIMAL,
                                        .usage =
                                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED

            };


              VmaAllocationCreateInfo allocCI = {};
              std::memset(&allocCI,0, sizeof(allocCI));
              allocCI.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
              allocCI.usage = VMA_MEMORY_USAGE_AUTO;

              vmaCreateImage(m_device.get_memory_allocator(), &depthImageCI, &allocCI, &m_depth_image,
                             &imageAllocation, nullptr);

            VkImageViewCreateInfo depthIvCI{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = m_depth_image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = depthFormat,
                .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                                  .levelCount = 1u,
                                  .layerCount = 1u}};
            vkCreateImageView(m_device.get_device_handle(), &depthIvCI, nullptr,
                              &m_depth_image_view);
          }
    }

    void SwapChain::createSyncObjects() {
        VkSemaphoreCreateInfo semCI{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VkFenceCreateInfo fenCI{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                .flags = VK_FENCE_CREATE_SIGNALED_BIT};

        m_fences.resize(MAX_FRAME_IN_FLIGHT);
        m_present_semaphores.resize(MAX_FRAME_IN_FLIGHT);

        for (uint32_t i = 0u; i < MAX_FRAME_IN_FLIGHT; ++i) {
            vkCreateFence(m_device.get_device_handle(), &fenCI, nullptr, &m_fences[i]);
            vkCreateSemaphore(m_device.get_device_handle(), &semCI, nullptr,
                              &m_present_semaphores[i]);
        }

        m_render_semaphores.resize(m_swap_chain_images.size());
        for (uint32_t i = 0; i < m_render_semaphores.size(); ++i) {
            vkCreateSemaphore(m_device.get_device_handle(), &semCI, nullptr,
                              &m_render_semaphores[i]);
        }
    }
}
