#ifndef __OCEAN_SCENE_H_GUARD
#define __OCEAN_SCENE_H_GUARD

#include <api/math.h>
#include <ocean/surface_geometry.h>
#include <ocean/surface_params.h>
#include <rendering/cubemap.h>
#include <rendering/quad.h>
#include <rendering/rendering_params.h>
#include <rendering/shader_effect.h>
#include <scene/camera.h>

namespace scene {

class ocean_scene {
public:
    ocean_scene(gpu::compute::command_queue queue, const ocean::surface_params& surface_params, const rendering::rendering_params& rendering_params);
    ocean_scene(const ocean_scene&) = delete;
    ocean_scene& operator=(const ocean_scene&) = delete;

    void render();
    camera& get_main_camera() { return main_camera; }
    const camera& get_main_camera() const { return main_camera; }

private:
    rendering::rendering_params rendering_params;
    camera main_camera;
    rendering::quad unit_quad;

    rendering::shader_effect ocean_effect;
    ocean::surface_geometry ocean_surface;

    rendering::shader_effect sky_effect;
    rendering::cubemap sky_env;
};

} // namespace scene

#endif // !__OCEAN_SCENE_H_GUARD
