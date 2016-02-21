#include <scene/camera_controller.h>

#include <scene/camera.h>

using math::real;

namespace scene {

void camera_controller::set_target(camera& target)
{
    this->target = &target;
    auto camera_vector = target.get_look_at() - target.get_position();
    radius =  math::length(camera_vector);
    orientation = math::spherical_angles(camera_vector);
}

void camera_controller::mouse_set_anchor(const mouse_pos_norm& pos)
{
    anchor_pos = pos;
}

void camera_controller::mouse_rotate(const mouse_pos_norm& pos)
{
    auto new_orientation = spherical_angles_from_mouse_pos(pos);
    auto camera_vector = radius * new_orientation.cartesian();
    target->set_position(target->get_look_at() - camera_vector);
}

void camera_controller::mouse_reset_anchor(const mouse_pos_norm& pos)
{
    orientation = spherical_angles_from_mouse_pos(pos);
}

void camera_controller::set_aspect_from_size(const os::window::size& size)
{
    if (!target) return;
    target->set_viewport_size(size);
}

math::spherical_angles camera_controller::spherical_angles_from_mouse_pos(const mouse_pos_norm& pos)
{
    auto pos_rel = pos - anchor_pos;
    real delta_azimuthal = math::pi * real(0.5) * pos_rel.x;
    real delta_polar = math::pi * real(0.25) * pos_rel.y;
    return { orientation.get_azimuthal() + delta_azimuthal, orientation.get_polar() + delta_polar };
}

} // namespace scene
