#include <scene/camera_controller.h>

#include <scene/camera.h>

using math::real;

namespace scene {

camera_controller::camera_controller(camera& target) : target(target), radius(0)
{
    auto camera_vector = target.get_look_at() - target.get_position();
    radius =  math::length(camera_vector);
    orientation = math::spherical_angles(camera_vector);
}

void camera_controller::set_viewport_size(const os::window::size& size)
{
    target.set_viewport_size(size);
}

void camera_controller::handle_event(const os::event& event)
{
    if (event.is_window_resize_event()) {
        set_viewport_size(event.get_window_size());
    } else if (event.is_mouse_button_event()) {
        handle_mouse_button_event(event.get_mouse_button_event());
    } else if (event.is_mouse_move_event()) {
        handle_mouse_move_event(event.get_mouse_move_event());
    }
}

void camera_controller::handle_mouse_button_event(const os::mouse_button_event& event)
{
    if (event.get_mouse_button() != os::mouse_button_event::MOUSE_BUTTON_LEFT)
        return;

    math::vec2 mouse_pos_relative = get_normalized_mouse_pos(event.get_mouse_button_pos());
    if (event.is_mouse_button_down()) {
        anchor_pos = mouse_pos_relative;
    } else {
        orientation = spherical_angles_from_mouse_pos(mouse_pos_relative);
    }
}

void camera_controller::handle_mouse_move_event(const os::mouse_move_event& event)
{
    if (event.get_mouse_move_button_state() & os::mouse_move_event::MOUSE_BUTTON_STATE_LEFT) {
        // Rotate view.
        math::vec2 mouse_pos_relative = get_normalized_mouse_pos(event.get_mouse_move_pos());
        auto new_orientation = spherical_angles_from_mouse_pos(mouse_pos_relative);
        auto camera_vector = radius * new_orientation.cartesian();
        target.set_position(target.get_look_at() - camera_vector);
    }
}

math::vec2 camera_controller::get_normalized_mouse_pos(const math::ivec2& mouse_pos)
{
    auto viewport_size = target.get_viewport_size();
    return { math::real(mouse_pos.x) / math::real(viewport_size.width),
             math::real(mouse_pos.y) / math::real(viewport_size.height) };
}

math::spherical_angles camera_controller::spherical_angles_from_mouse_pos(const mouse_pos_norm& pos)
{
    auto pos_rel = pos - anchor_pos;
    real delta_azimuthal = math::pi * real(0.5) * pos_rel.x;
    real delta_polar = math::pi * real(0.25) * pos_rel.y;
    return { orientation.get_azimuthal() + delta_azimuthal, orientation.get_polar() + delta_polar };
}

} // namespace scene
