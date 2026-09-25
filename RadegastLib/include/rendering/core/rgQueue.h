#ifndef RADEGAST_RGQUEUE_H
#define RADEGAST_RGQUEUE_H

#include <vulkan/vulkan.h>
#include <cstdint>

namespace rg {
    struct Queue {
        VkQueue handle{VK_NULL_HANDLE};
        uint32_t idx {-1u};
    };
}

#endif //RADEGAST_RGQUEUE_H
