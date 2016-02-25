#ifndef __CAMERA_H_GUARD
#define __CAMERA_H_GUARD

#include <api/math.h>
#include <util/cached_data.h>
#include <util/rect.h>

namespace scene {

class camera {
public:
    camera() : position(0, 0, -1), look_at(0, 0, 0), up_vector(0, 1, 0),
               viewport_size(1, 1), fov_y(math::pi * math::real(0.5)), z_near(0.5), z_far(10000) {}

    const math::vec3& get_position() const { return position; }
    const math::vec3& get_look_at() const { return look_at; }
    const math::vec3& get_up_vector() const { return up_vector; }
    void set_position(math::vec3 position) { invalidate_view_matrix(); this->position = position; }
    void set_look_at(math::vec3 look_at) { invalidate_view_matrix(); this->look_at = look_at; }
    void set_up_vector(math::vec3 up_vector) { invalidate_view_matrix(); this->up_vector = up_vector; }

    util::extent get_viewport_size() const { return viewport_size; }
    math::real get_aspect() const { return math::real(viewport_size.width) / math::real(viewport_size.height); }
    math::vec2 get_eye_size() const { auto hgt = math::real(2) * math::tan(math::real(0.5) * fov_y); return { hgt * get_aspect(), hgt }; }
    math::real get_fov_y() const { return fov_y; }
    math::real get_z_near() const { return z_near; }
    math::real get_z_far() const { return z_far; }
    void set_viewport_size(util::extent viewport_size) { invalidate_proj_matrix(); this->viewport_size = viewport_size; }
    void set_fov_y(math::real fov) { invalidate_proj_matrix(); this->fov_y = fov; }
    void set_z_near(math::real z_near) { invalidate_proj_matrix(); this->z_near = z_near; }
    void set_z_far(math::real z_far) { invalidate_proj_matrix(); this->z_far = z_far; }

    inline math::mat4 get_view_matrix() const;
    inline math::mat4 get_proj_matrix() const;
    inline math::mat4 get_proj_view_matrix() const;

private:
    void invalidate_view_matrix() const { view_matrix.invalidate(); proj_view_matrix.invalidate(); }
    void invalidate_proj_matrix() const { proj_matrix.invalidate(); proj_view_matrix.invalidate(); }
    void update_view_matrix() const;
    void update_proj_matrix() const;

    math::vec3 position;
    math::vec3 look_at;
    math::vec3 up_vector;
    util::extent viewport_size;
    math::real fov_y; // radians
    math::real z_near, z_far;

    mutable util::cached_data<math::mat4> view_matrix, proj_matrix, proj_view_matrix;
};

inline math::mat4 camera::get_view_matrix() const
{
    if (view_matrix.is_dirty())
        update_view_matrix();

    return view_matrix.get();
}

inline math::mat4 camera::get_proj_matrix() const
{
    if (proj_matrix.is_dirty())
        update_proj_matrix();

    return proj_matrix.get();
}

inline math::mat4 camera::get_proj_view_matrix() const
{
    if (proj_view_matrix.is_dirty())
        proj_view_matrix = get_proj_matrix() * get_view_matrix();

    return proj_view_matrix.get();
}

} // namespace scene

#endif // !__CAMERA_H_GUARD
