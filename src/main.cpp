#include <main_window.h>
#include <rendering/rendering_params.h>

int main(int argc, char *argv[])
{
    rendering::rendering_params rendering_params;
    rendering_params.texture_max_anisotropy = 2;
    rendering_params.multisampling_sample_count = 4;
    rendering_params.projected_grid_size_factor = math::vec2(0.5f, 0.5f); // relative to window size

    ocean::surface_params ocean_params;
    ocean_params.fft_size = math::ivec2(1024, 1024);
    ocean_params.tile_size_logical = math::vec3(100, 100, 100);
    ocean_params.tile_size_physical = math::vec3(200, 200, 200);
    ocean_params.amplitude_factor = 3.0e-6f;
    ocean_params.wavelength_low_threshold = math::real(0.7);
    ocean_params.set_wind_vector(math::vec2(15, 0));

    main_window main_window(rendering_params, ocean_params);

    main_window.main_loop();

    return EXIT_SUCCESS;
}
