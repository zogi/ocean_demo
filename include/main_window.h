#ifndef __MAIN_WINDOW_H_GUARD
#define __MAIN_WINDOW_H_GUARD

#include <api/os/window.h>
#include <rendering/framebuffer.h>
#include <scene/camera_controller.h>

class main_window {
public:
    enum run_state { RUN_STATE_UNINITIALIZED, RUN_STATE_RUNNING, RUN_STATE_QUITTING };

    main_window(int width, int height, const char *title, int num_framebuffer_samples);
    main_window(const main_window&) = delete;
    main_window& operator=(const main_window&) = delete;

    void process_events();
    void swap_frame();
    void attach_camera(scene::camera& camera);
    run_state get_run_state() const { return run_state; }
    auto get_graphics_context() const { return window.get_graphics_context(); }

private:
    void handle_quit_event();
    void handle_resize_event(const os::window::size& size);
    void handle_keyboard_event(const os::keyboard_event& event);

    os::window window;
    rendering::multisample_framebuffer framebuffer;
    scene::camera_controller camera_controller;
    run_state run_state;
};

#endif // !__MAIN_WINDOW_H_GUARD
