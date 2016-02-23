#include <main_window.h>
#include <rendering/rendering_params.h>
#include <scene/camera_controller.h>
#include <scene/ocean_scene.h>
#include <util/log.h>

int main(int argc, char *argv[])
{
    rendering::rendering_params rendering_params;
    rendering_params.texture_max_anisotropy = 2;
    rendering_params.multisampling_sample_count = 4;
    rendering_params.projected_grid_size_factor = math::vec2(0.25f, 0.25f); // relative to window size

    ocean::surface_params ocean_params;
    ocean_params.fft_size = math::ivec2(512, 512);
    ocean_params.tile_size_logical = math::vec3(10, 10, 10);
    ocean_params.tile_size_physical = math::vec3(20, 20, 20);
    ocean_params.amplitude_factor = 1.0e-3f;
    ocean_params.wavelength_low_threshold = math::real(0.25);
    ocean_params.set_wind_vector(math::vec2(15, 0));

    main_window main_window(rendering_params, ocean_params);

    main_window.main_loop();

    return EXIT_SUCCESS;
}
