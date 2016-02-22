#ifndef __CAMERA_CONRTOLLER_H_GUARD
#define __CAMERA_CONRTOLLER_H_GUARD

#include <api/math.h>
#include <api/os/event.h>
#include <api/os/window.h>

namespace scene {

class camera;

class camera_controller {
public:
    // Normalized mouse position; the coordinates are in [0, 1].
    typedef math::vec2 mouse_pos_norm;

    camera_controller::camera_controller() : target(nullptr), radius(0) {}
    bool has_target() const { return target != nullptr; }
    void set_target(camera& target);
    void set_viewport_size(const math::ivec2& size);

    void handle_event(const os::event& event);
    void handle_mouse_button_event(const os::mouse_button_event& event);
    void handle_mouse_move_event(const os::mouse_move_event & event);

private:
    math::spherical_angles spherical_angles_from_mouse_pos(const mouse_pos_norm& pos);

    camera *target;
    math::real radius;
    math::spherical_angles orientation;
    mouse_pos_norm anchor_pos;
};

} // namespace scene

#endif // !__CAMERA_CONRTOLLER_H_GUARD
