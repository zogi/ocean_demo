#include <main_window.h>

main_window::main_window(int width, int height, const char *title, int num_framebuffer_samples)
    : window(width, height, title),
      framebuffer(width, height, num_framebuffer_samples),
      run_state(RUN_STATE_RUNNING)
{
}

bool main_window::poll_event(os::event& event)
{
    if (!window.poll_event(event))
        return false;

    if (event.is_quit_event()) {
        handle_quit_event();
    } else if (event.is_window_resize_event()) {
        handle_resize_event(event.get_window_size());
    } else if (event.is_keyboard_event()) {
        handle_keyboard_event(event.get_keyboard_event());
    }

    return true;
}

void main_window::swap_frame()
{
    framebuffer.resolve_to_backbuffer();
    window.swap_frame();
    framebuffer.activate();
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
