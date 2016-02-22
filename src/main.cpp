#include <main_window.h>
#include <rendering/rendering_params.h>
#include <scene/camera_controller.h>
#include <scene/ocean_scene.h>
#include <util/log.h>

int main(int argc, char *argv[])
{
    main_window main_window(640, 480, "ocean demo", 4);

    gpu::compute::command_queue queue = gpu::compute::init(main_window.get_graphics_context());

    ocean::surface_params params;
    params.tile_size_logical = math::vec3(10, 10, 10);
    params.tile_size_physical = math::vec3(20, 20, 20);
    params.grid_size = math::ivec2(512, 512);
    params.amplitude_factor = 1.0e-3f;
    params.wavelength_low_threshold = math::real(0.25);
    params.set_wind_vector(math::vec2(15, 0));

    LOG("Initializing Tessendorf heightfield.\n");
    scene::ocean_scene ocean_scene(queue, params);

    scene::camera_controller camera_controller(ocean_scene.get_main_camera());
    camera_controller.set_viewport_size(main_window.get_size());

    LOG("Starting main loop.\n");
    while (main_window.get_run_state() == main_window::RUN_STATE_RUNNING) {
        for (auto event : main_window.unprocessed_events()) {
            camera_controller.handle_event(event);
        }
        ocean_scene.render();
        main_window.swap_frame();
    }

    return EXIT_SUCCESS;
}
