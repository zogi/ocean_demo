#include <main_window.h>

main_window::main_window(const rendering::rendering_params& rendering_params, const ocean::surface_params& ocean_params)
  : window("ocean demo", os::window::size(1024, 768), SDL_WINDOW_MAXIMIZED),
    queue(gpu::compute::init(window.get_graphics_context())),
    framebuffer(window.get_size().x, window.get_size().y, rendering_params.multisampling_sample_count),
    ocean_scene(queue, ocean_params), // TODO: pass rendering params to ctor. Then the camera should only store aspect and not viewport size.
    camera_controller(ocean_scene.get_main_camera()),
    run_state(RUN_STATE_RUNNING)
{
    camera_controller.set_viewport_size(get_size());
}

void main_window::main_loop()
{
    while (run_state == RUN_STATE_RUNNING) {
        for (auto event : window.unprocessed_events()) {
            handle_event(event);
            camera_controller.handle_event(event);
        }

        framebuffer.activate();
        ocean_scene.render();
        framebuffer.resolve_to_backbuffer();

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

void main_window::handle_resize_event(const os::window::size& size)
{
    gpu::graphics::set_viewport_size(size.get_width(), size.get_height());
    framebuffer.resize(size.get_width(), size.get_height());
}

void main_window::handle_keyboard_event(const os::keyboard_event& event)
{
    const char KEY_ESCAPE = '\033';
    if (event.get_keysym() == KEY_ESCAPE)
        handle_quit_event();
}
