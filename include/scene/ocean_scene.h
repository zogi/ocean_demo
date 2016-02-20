#ifndef __OCEAN_SCENE_H_GUARD
#define __OCEAN_SCENE_H_GUARD

#include <api/math.h>
#include <ocean/displacement_map.h>
#include <ocean/spectrum.h>
#include <rendering/quad.h>
#include <rendering/shader_effect.h>
#include <rendering/cubemap.h>
#include <scene/camera.h>

namespace scene {

class ocean_scene {
public:
    ocean_scene() {}
    ocean_scene(const ocean_scene&) = delete;
    ocean_scene& operator=(const ocean_scene&) = delete;

    void init(ocean::spectrum *ocean_spectrum, math::real units_per_meter);
    void render();
    camera& get_main_camera() { return main_camera; }
    const camera& get_main_camera() const { return main_camera; }

private:
    camera main_camera;
    rendering::quad unit_quad;

    rendering::shader_effect ocean_effect;
    ocean::displacement_map ocean_surface;

    rendering::shader_effect sky_effect;
    rendering::cubemap sky_env;
};

} // namespace scene

#endif // !__OCEAN_SCENE_H_GUARD
