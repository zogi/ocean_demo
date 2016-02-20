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
    void set_target(camera *target);

    void set_aspect_from_size(const os::window::size& size);

    void mouse_set_anchor(const mouse_pos_norm& pos);
    void mouse_rotate(const mouse_pos_norm& pos);
    void mouse_reset_anchor(const mouse_pos_norm& pos);

private:
    math::spherical_angles spherical_angles_from_mouse_pos(const mouse_pos_norm& pos);

    camera *target;
    math::real radius;
    math::spherical_angles orientation;
    mouse_pos_norm anchor_pos;
};

} // namespace scene

#endif // !__CAMERA_CONRTOLLER_H_GUARD
