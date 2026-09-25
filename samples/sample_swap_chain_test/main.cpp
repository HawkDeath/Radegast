#include <platform/rgWindow.h>
#include <rendering/core/rgDevice.h>
#include <rendering/core/rgInstance.h>
#include <rendering/core/rgPhysicalDevice.h>
#include <rendering/core/rgSwapChain.h>
#include <iostream>
#include <vector>

int main() {
    try {
        VkExtent2D window_size {1920u, 1080u};
        rg::Window win_test{window_size.width, window_size.height, "0_3_sample_swap_chain_test"};
        rg::Instance instance {};
        const auto gpus = instance.get_available_physical_device_list();
        if (gpus.empty()) return -2;

        const rg::PhysicalDevice& physical_device{gpus[0]};
        rg::Device device{win_test, instance, physical_device};
        rg::SwapChain swap_chain {device, window_size};

        while (!win_test.should_close()) {
            win_test.update_events();

            if (auto cmd = swap_chain.beginFrame()) {
                swap_chain.endFrame(cmd);
                swap_chain.submit(cmd);
            }

        }

    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
