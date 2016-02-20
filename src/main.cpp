#include <rendering/main_window.h>
#include <scene/ocean_scene.h>
#include <util/log.h>

constexpr int heightfield_size = 512;
const float meters_per_unit = 20.0f;
const float damp_length = 0.25f;
const float A = 1.0e-3f;
const float wind_x = 15.0f;
const float wind_y = 0.0f;

int main(int argc, char *argv[])
{
    rendering::main_window main_window;
    main_window.init(640, 480, "demo", 4);

    gpu::compute compute(main_window.get_graphics_context());

    LOG("Initializing Tessendorf heightfield.\n");
    const int N = heightfield_size, M = heightfield_size;
    ocean::spectrum ocean_spectrum;
    ocean_spectrum.set_params(meters_per_unit, meters_per_unit, N, M, A, damp_length, wind_x, wind_y);
    ocean_spectrum.bake_params(&compute);

    scene::ocean_scene ocean_scene;
    ocean_scene.init(&ocean_spectrum, 1 / meters_per_unit);
    main_window.set_camera(ocean_scene.get_main_camera());

    LOG("Starting main loop.\n");
    while (main_window.get_run_state() == rendering::main_window::RUN_STATE_RUNNING) {
        main_window.process_events();
        ocean_scene.render();
        main_window.swap_frame();
    }

    return EXIT_SUCCESS;
}
