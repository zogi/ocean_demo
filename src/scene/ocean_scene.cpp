#include <chrono>
#include <scene/ocean_scene.h>
#include <util/log.h>

using namespace rendering::shader_type;

namespace scene {

namespace {

constexpr gpu::graphics::texture_unit ocean_displacement_tex_unit = 0;
constexpr gpu::graphics::texture_unit ocean_height_deriv_tex_unit = 1;
constexpr gpu::graphics::texture_unit sky_cubemap_tex_unit = 2;

} // unnamed namespace

ocean_scene::ocean_scene(
    gpu::compute::command_queue queue,
    const ocean::surface_params &surface_params,
    const rendering::rendering_params &rendering_params)
    : rendering_params(rendering_params), ocean_surface(queue, surface_params)
{
    main_camera.set_look_at(math::vec3(0, 1, 0));
    main_camera.set_position(math::vec3(0, 14, 30));

    // Ocean
    ocean_surface.set_texture_max_anisotropy(rendering_params.texture_max_anisotropy);
    ocean_surface.bind_displacement_texture(ocean_displacement_tex_unit);
    ocean_surface.bind_height_gradient_texture(ocean_height_deriv_tex_unit);

    ocean_effect.load_shaders("shaders/ocean.glsl", VERTEX | GEOMETRY | FRAGMENT);
    ocean_effect.use();
    ocean_effect.set_parameter(
        "units_per_meter", surface_params.tile_size_logical / surface_params.tile_size_physical);
    ocean_effect.set_parameter("tile_size_logical", surface_params.tile_size_logical);
    ocean_effect.set_parameter("displacement_tex", ocean_displacement_tex_unit);
    ocean_effect.set_parameter("normal_tex", ocean_height_deriv_tex_unit);
    ocean_effect.set_parameter("sky_env", sky_cubemap_tex_unit);

    // Sky
    try {
        sky_env.load_from_file("textures/sky.png");
    } catch (rendering::cubemap::load_error) {
        sky_env.load_from_file("textures/sky-default.png");
    }
    sky_env.bind(sky_cubemap_tex_unit);

    sky_effect.load_shaders("shaders/sky.glsl", VERTEX | FRAGMENT);
    sky_effect.set_z_test_state(false);
    sky_effect.set_z_write_state(false);
    sky_effect.use();
    sky_effect.set_parameter("sky_env", sky_cubemap_tex_unit);

    // Initial gui state.
    {
        gui_state.amplitude = surface_params.amplitude;
        gui_state.wind_speed = surface_params.wind_speed;
        const auto v = surface_params.wind_direction;
    }
}

void ocean_scene::render()
{
    // Gui
    {
        ImGui::Begin("ocean params");

        ImGui::SliderFloat("amplitude", &gui_state.amplitude, 0, 5);
        ocean_surface.set_wave_amplitude(gui_state.amplitude);

        const bool wind_changed = ImGui::SliderFloat("wind speed", &gui_state.wind_speed, 0, 20);
        if (wind_changed) {
            math::vec2 v = math::normalize(ocean_surface.get_wind_vector());
            ocean_surface.set_wind_vector(v * gui_state.wind_speed);
        }

        ImGui::End();
    }

    // Scene
    static auto start_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    float time =
        0.001f *
        std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

    ocean_surface.enqueue_generate(time);
    timings.surface_geometry_timing_data = ocean_surface.get_timing_data();

    timer.start();

    glDepthMask(GL_TRUE);
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
    math::ivec2 grid_dim =
        math::vec2(viewport_size.width, viewport_size.height) / rendering_params.tile_size_pixels;

    ocean_effect.use();
    ocean_effect.set_parameter("camera.internal.viewport_size", viewport_size);
    // Increase grid size to hide displacement holes due to waves at the edges of
    // the screen.
    ocean_effect.set_parameter("camera.internal.view_size", math::real(1.2) * eye_size);
    ocean_effect.set_parameter("camera.internal.z_far", main_camera.get_z_far());
    ocean_effect.set_parameter("camera.model_transform.position", main_camera.get_position());
    ocean_effect.set_parameter("camera.model_transform.orientation", camera_orientation);
    ocean_effect.set_parameter("grid_dim", grid_dim);
    ocean_effect.set_parameter("proj_view_world_transform", proj_view_world);

    double render_except_ocean_drawcall = timer.stop_and_get_milliseconds();

    {
        auto ocean_timer = util::scoped_timer(timer, timings.ocean_drawcall_milliseconds);
        unit_quad.draw_instanced(grid_dim.x * grid_dim.y);
    }

    timings.render_milliseconds = render_except_ocean_drawcall + timings.ocean_drawcall_milliseconds;
}

} // namespace scene
