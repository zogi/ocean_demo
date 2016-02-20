#include <rendering/main_window.h>

namespace rendering {

main_window::main_window(int width, int height, const char *title, int num_framebuffer_samples)
    : window(width, height, title), event_handler(*this), run_state(RUN_STATE_RUNNING)
{
    framebuffer.allocate(width, height, num_samples);
}

main_window::~main_window()
{
}

void main_window::set_camera(scene::camera& camera)
{
    main_camera_controller.set_target(&camera);
    main_camera_controller.set_aspect_from_size(window.get_size());
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
    } else if (event.is_mouse_button_event()) {
        parent.handle_mouse_button_event(event.get_mouse_button_event());
    } else if (event.is_mouse_move_event()) {
        parent.handle_mouse_move_event(event.get_mouse_move_event());
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
    main_camera_controller.set_aspect_from_size(size);
}

void main_window::handle_keyboard_event(const os::keyboard_event& event)
{
    const char KEY_ESCAPE = '\033';
    if (event.get_keysym() == KEY_ESCAPE)
        handle_quit_event();
}

void main_window::handle_mouse_button_event(const os::mouse_button_event& event)
{
    if (event.get_mouse_button() != os::mouse_button_event::MOUSE_BUTTON_LEFT)
        return;

    math::vec2 window_size = get_size();
    math::vec2 mouse_pos = event.get_mouse_button_pos();
    if (event.is_mouse_button_down()) {
        main_camera_controller.mouse_set_anchor(mouse_pos / window_size);
    } else {
        main_camera_controller.mouse_reset_anchor(mouse_pos / window_size);
    }
}

void main_window::handle_mouse_move_event(const os::mouse_move_event & event)
{
    math::vec2 window_size = get_size();
    math::vec2 mouse_pos = event.get_mouse_move_pos();
    if (event.get_mouse_move_button_state() & os::mouse_move_event::MOUSE_BUTTON_STATE_LEFT) {
        main_camera_controller.mouse_rotate(mouse_pos / window_size);
    }
}

} // namespace rendering
