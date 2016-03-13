#ifndef __MAIN_WINDOW_H_GUARD
#define __MAIN_WINDOW_H_GUARD

#include <api/os/window.h>
#include <api/gpu/compute.h>
#include <ocean/surface_params.h>
#include <rendering/framebuffer.h>
#include <rendering/rendering_params.h>
#include <rendering/text_renderer.h>
#include <scene/camera_controller.h>
#include <scene/ocean_scene.h>

class main_window {
public:
    enum run_state { RUN_STATE_UNINITIALIZED, RUN_STATE_RUNNING, RUN_STATE_QUITTING };

    main_window(const rendering::rendering_params& rendering_params, const ocean::surface_params& ocean_params);
    main_window(const main_window&) = delete;
    main_window& operator=(const main_window&) = delete;

    void main_loop();
    util::extent get_extent() const { return window.get_extent(); }

private:
    void handle_event(const os::event& event);
    void handle_quit_event();
    void handle_resize_event(const util::extent& new_extent);
    void handle_keyboard_event(const os::keyboard_event& event);

    void render_performance_metrics(double multisample_resolve_milliseconds);

    os::window window;
    gpu::compute::command_queue queue;
    rendering::multisample_framebuffer framebuffer;
    rendering::text_renderer text_renderer;
    scene::ocean_scene ocean_scene;
    scene::camera_controller camera_controller;
    run_state run_state;
};

#endif // !__MAIN_WINDOW_H_GUARD
