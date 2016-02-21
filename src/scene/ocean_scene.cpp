#include <scene/ocean_scene.h>
#include <chrono>
#include <util/log.h>

using namespace rendering::shader_type;

namespace scene {

namespace {

constexpr gpu::graphics::texture_unit ocean_displacement_tex_unit = 0;
constexpr gpu::graphics::texture_unit ocean_height_deriv_tex_unit = 1;
constexpr gpu::graphics::texture_unit sky_cubemap_tex_unit = 2;

} // unnamed namespace


ocean_scene::ocean_scene(gpu::compute::command_queue queue, const ocean::surface_params& params)
  : ocean_surface(queue, params)
{
    main_camera.set_look_at(math::vec3(0, 1, 0));
    main_camera.set_position(math::vec3(0, 4, 6));

    unit_quad.init();

    // Ocean
    ocean_surface.bind_displacement_texture(ocean_displacement_tex_unit);
    ocean_surface.bind_height_gradient_texture(ocean_height_deriv_tex_unit);

    ocean_effect.load_shaders("shaders/ocean.glsl", VERTEX | GEOMETRY | FRAGMENT);
    ocean_effect.use();
    ocean_effect.set_parameter("units_per_meter", params.tile_size_logical / params.tile_size_physical);
    ocean_effect.set_parameter("tile_size_logical", params.tile_size_logical);
    ocean_effect.set_parameter("displacement_tex", ocean_displacement_tex_unit);
    ocean_effect.set_parameter("d_height_tex", ocean_height_deriv_tex_unit);
    ocean_effect.set_parameter("sky_env", sky_cubemap_tex_unit);

    // Sky
    sky_env.load_from_file("textures/sky.png");
    sky_env.bind(sky_cubemap_tex_unit);

    sky_effect.load_shaders("shaders/sky.glsl", VERTEX | FRAGMENT);
    sky_effect.set_z_test_state(false);
    sky_effect.set_z_write_state(false);
    sky_effect.use();
    sky_effect.set_parameter("sky_env", sky_cubemap_tex_unit);

}

void ocean_scene::render()
{
    static auto start_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    float time = 0.001f * std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

    ocean_surface.enqueue_generate(time);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto camera_orientation = math::transpose(math::mat3(main_camera.get_view_matrix()));

    // Sky
    auto eye_size = main_camera.get_eye_size();
    sky_effect.use();
    sky_effect.set_parameter("view_rotation", camera_orientation);
    sky_effect.set_parameter("eye_size", eye_size);
    unit_quad.draw();

    // Ocean
    auto proj_view_world = main_camera.get_proj_view_matrix();
    auto viewport_size = main_camera.get_viewport_size();
    math::ivec2 grid_dim = viewport_size / 4;

    ocean_effect.use();
    ocean_effect.set_parameter("camera.internal.viewport_size", viewport_size);
    // Increase grid size to hide displacement holes due to waves at the edges of the screen.
    ocean_effect.set_parameter("camera.internal.view_size", math::real(1.2) * eye_size);
    ocean_effect.set_parameter("camera.internal.z_far", main_camera.get_z_far());
    ocean_effect.set_parameter("camera.model_transform.position", main_camera.get_position());
    ocean_effect.set_parameter("camera.model_transform.orientation", camera_orientation);
    ocean_effect.set_parameter("grid_dim", grid_dim);
    ocean_effect.set_parameter("proj_view_world_transform", proj_view_world);
    unit_quad.draw_instanced(grid_dim.x * grid_dim.y);
}

} // namespace scene
