#include <rgWindow.h>


int main() {

    rg::Window win_test;
    while (!win_test.should_close())
    {
        win_test.update_events();
    }
    return 0;
}