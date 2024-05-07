#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

int main() {
    uint32_t vulkan_api_version = 0u;
    vkEnumerateInstanceVersion(&vulkan_api_version);

    spdlog::info("VulkanAPI version {}.{}.{}", VK_API_VERSION_MAJOR(vulkan_api_version),
                 VK_API_VERSION_MINOR(vulkan_api_version),
                 VK_API_VERSION_PATCH(vulkan_api_version));

    glfwSetErrorCallback([](int code, const char *desc) -> void {
        spdlog::error("[GLFW_error] {} - {}", code, desc);
    });
    glfwInit();

    auto mat_res = glm::mat4(1.0f) * glm::vec4(0.5f);
    spdlog::info("mat_result:\n\t\t\t\t{}, {}, {}\n\t\t\t\t{}, {}, {}\n\t\t\t\t{}, {}, {}", mat_res[0], mat_res[1],
                 mat_res[2], mat_res[3],
                 mat_res[4], mat_res[5], mat_res[6], mat_res[7], mat_res[8]);

    glfwTerminate();

    return 0;
}