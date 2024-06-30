#ifndef RGDEVICE_H
#define RGDEVICE_H

#include <vulkan/vulkan.h>

namespace rg
{
class rgDevice
{
public:
    // input: extensions to enable, reference to window (for surface creation)
    rgDevice();
    ~rgDevice();

    VkDevice get_device_handle() const { return m_device; }

private:

    VkDevice m_device;
};
}

#endif //RGDEVICE_H
