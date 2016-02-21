#include <rendering/main_window.h>
#include <scene/ocean_scene.h>
#include <util/log.h>

int main(int argc, char *argv[])
{
    rendering::main_window main_window(640, 480, "demo", 4);

    gpu::compute compute(main_window.get_graphics_context());

    ocean::surface_params params;
    params.tile_size_logical = math::vec3(10, 10, 10);
    params.tile_size_physical = math::vec3(20, 20, 20);
    params.grid_size = math::ivec2(512, 512);
    params.amplitude_factor = 1.0e-3f;
    params.wavelength_low_threshold = math::real(0.25);
    params.set_wind_vector(math::vec2(15, 0));

    LOG("Initializing Tessendorf heightfield.\n");
    scene::ocean_scene ocean_scene(compute.get_command_queue(), params, 1.0f / 20.0f);
    main_window.set_camera(ocean_scene.get_main_camera());

    LOG("Starting main loop.\n");
    while (main_window.get_run_state() == rendering::main_window::RUN_STATE_RUNNING) {
        main_window.process_events();
        ocean_scene.render();
        main_window.swap_frame();
    }

    return EXIT_SUCCESS;
}
