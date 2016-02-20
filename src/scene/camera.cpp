#include <scene/camera.h>

namespace scene {

void camera::update_view_matrix() const
{
    view_matrix.set(math::lookAt(position, look_at, up_vector));
}

void camera::update_proj_matrix() const
{
    proj_matrix.set(math::perspective(fov_y, get_aspect(), z_near, z_far));
}

} // namespace scene
