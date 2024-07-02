#ifndef RGINSTANCE_H
#define RGINSTANCE_H

#include <common/rgHelper.h>

#include <vulkan/vulkan.h>
#include <vector>

namespace rg
{

class Instance {
public:
    Instance(ExtensionMap instance_extensions = {});
    ~Instance();

    [[nodiscard]] VkInstance get_instance_handler() const { return m_instance; }
    std::vector<VkPhysicalDevice> get_available_physical_device_list();

private:
    void create_instance(ExtensionMap &instance_extensions);
private:
    VkInstance m_instance;

};
}

#endif //RGINSTANCE_H
