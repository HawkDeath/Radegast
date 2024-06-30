#include <rendering/core/rgDevice.h>

namespace rg {
    rgDevice::rgDevice() {

    }
    rgDevice::~rgDevice() {
        vkDestroyDevice(m_device, nullptr);
    }


}