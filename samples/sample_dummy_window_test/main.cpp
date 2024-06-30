#include <platform/rgWindow.h>


int main() {
    rg::Window win_test{1920u, 1080u, "0_1_sample_dummy_window_test"};
    while (!win_test.should_close()) {
        rg::Window::update_events();
    }
    return 0;
}
