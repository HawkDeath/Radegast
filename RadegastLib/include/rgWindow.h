#ifndef RADEGAST_RGWINDOW_H
#define RADEGAST_RGWINDOW_H

#include <GLFW/glfw3.h>

namespace rg {
    class Window {
    public:
        Window();

        ~Window();

        bool should_close();

        void update_events();

    private:
        GLFWwindow *window_handler;
    };
}

#endif //RADEGAST_RGWINDOW_H
