#include <platform/rgWindow.h>
#include <rendering/core/rgDevice.h>
#include <rendering/core/rgInstance.h>
#include <rendering/core/rgPhysicalDevice.h>
#include <iostream>
#include <vector>

int main() {
    try {
        rg::Window win_test{1920u, 1080u, "0_2_sample_device_instance_test"};
        rg::Instance instance {};
        auto gpus = instance.get_available_physical_device_list();
        if (gpus.empty()) return -2;

        rg::PhysicalDevice physical_device{gpus[0]};
        rg::Device device{win_test, instance, physical_device};
    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
