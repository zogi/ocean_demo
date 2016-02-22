#include <main_window.h>

main_window::main_window(int width, int height, const char *title, int num_framebuffer_samples)
    : window(width, height, title), event_handler(*this),
      framebuffer(width, height, num_framebuffer_samples),
      run_state(RUN_STATE_RUNNING)
{
}

void main_window::attach_camera(scene::camera& camera)
{
    main_camera_controller.set_target(camera);
    main_camera_controller.set_viewport_size(window.get_size());
}

void main_window::swap_frame()
{
    framebuffer.resolve_to_backbuffer();
    window.swap_frame();
}

void main_window::event_handler::handle_event(const os::event& event)
{
    if (event.is_quit_event()) {
        parent.handle_quit_event();
    } else if (event.is_window_resize_event()) {
        parent.handle_resize_event(event.get_window_size());
    } else if (event.is_keyboard_event()) {
        parent.handle_keyboard_event(event.get_keyboard_event());
    }

    parent.main_camera_controller.handle_event(event);
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
