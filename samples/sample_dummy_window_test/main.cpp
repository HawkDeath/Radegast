#include <platform/rgWindow.h>


int main() {

    rg::Window win_test;
    while (!win_test.should_close())
    {
         rg::Window::update_events();
    }
    return 0;
}