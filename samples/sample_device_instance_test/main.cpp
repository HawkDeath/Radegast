#include <platform/rgWindow.h>
#include <rendering/core/rgDevice.h>
#include <iostream>

int main() {

    try {
        rg::Window win_test{1920u, 1080u, "0_1_sample_dummy_window_test"};
        rg::rgDevice device{win_test};
    } catch (std::exception &e) {
        std::cerr<< "exception: " << e.what() << std::endl;
        return  -1;
    }

    return 0;
}
