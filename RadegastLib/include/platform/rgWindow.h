#ifndef RADEGAST_RGWINDOW_H
#define RADEGAST_RGWINDOW_H

struct GLFWwindow;

namespace rg {
    class Window {
    public:
        Window();

        ~Window();

        bool should_close();

        static void update_events();

    private:
        GLFWwindow *window_handler;
    };
}

#endif //RADEGAST_RGWINDOW_H
