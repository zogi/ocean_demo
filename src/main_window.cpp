#include <main_window.h>

#include <sstream>
#include <util/timing.h>

main_window::main_window(const rendering::rendering_params& rendering_params, const ocean::surface_params& ocean_params)
  : window("ocean demo", util::extent(1024, 768), SDL_WINDOW_MAXIMIZED),
    queue(gpu::compute::init(window.get_graphics_context())),
    framebuffer(window.get_extent().width, window.get_extent().height, rendering_params.multisampling_sample_count),
    ocean_scene(queue, ocean_params, rendering_params),
    camera_controller(ocean_scene.get_main_camera()),
    run_state(RUN_STATE_RUNNING)
{
    camera_controller.set_viewport_size(get_extent());
}

void main_window::main_loop()
{
    double multisample_resolve_milliseconds;
    util::graphics_timer timer;

    while (run_state == RUN_STATE_RUNNING) {
        os::event event;
        while (window.poll_event(event)) {
            handle_event(event);
            camera_controller.handle_event(event);
        }

        framebuffer.activate();
        ocean_scene.render();

        {
            auto resolve_timer = util::scoped_timer(timer, multisample_resolve_milliseconds);
            framebuffer.resolve_to_backbuffer();
        }

        // Render performance metrics.
        render_performance_metrics(multisample_resolve_milliseconds);

        window.swap_frame();
    }
}

void main_window::handle_event(const os::event& event)
{
    if (event.is_quit_event()) {
        handle_quit_event();
    } else if (event.is_window_resize_event()) {
        handle_resize_event(event.get_window_size());
    } else if (event.is_keyboard_event()) {
        handle_keyboard_event(event.get_keyboard_event());
    }
}

void main_window::handle_quit_event()
{
    run_state = RUN_STATE_QUITTING;
}

void main_window::handle_resize_event(const util::extent& size)
{
    gpu::graphics::set_viewport_size(size.width, size.height);
    framebuffer.resize(size.width, size.height);
}

void main_window::handle_keyboard_event(const os::keyboard_event& event)
{
    const char KEY_ESCAPE = '\033';
    if (event.get_keysym() == KEY_ESCAPE)
        handle_quit_event();
}

void main_window::render_performance_metrics(double multisample_resolve_milliseconds)
{
    auto ocean_timing_data = ocean_scene.get_timing_data();
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);
    ss << "FFT: TODO ms\n";
    ss << "ocean surface: " << ocean_timing_data.ocean_drawcall_milliseconds << " ms\n";
    ss << "rendering total: " << ocean_timing_data.render_milliseconds << " ms\n";
    ss << "framebuffer resolve: " << multisample_resolve_milliseconds << " ms\n";
    text_renderer.render_text(ss.str(), util::offset(10, 10));
}
