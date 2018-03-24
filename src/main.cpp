#include <main_window.h>
#include <rendering/rendering_params.h>

int main(int argc, char *argv[])
{
    rendering::rendering_params rendering_params;
    rendering_params.texture_max_anisotropy = 2;
    rendering_params.multisampling_sample_count = 4;
    rendering_params.tile_size_pixels = math::vec2(4, 4);

    ocean::surface_params ocean_params;
    ocean_params.fft_size = math::ivec2(1024, 1024);
    ocean_params.tile_size_logical = math::vec3(100, 100, 100);  // For rendering.
    ocean_params.tile_size_physical = math::vec3(200, 200, 200); // For heightmap generation.
    ocean_params.amplitude = 1.0e-3;
    ocean_params.wavelength_low_threshold = math::real(0.7);
    ocean_params.set_wind_vector(math::vec2(15, 0));

    main_window main_window(rendering_params, ocean_params);

    main_window.main_loop();

    return EXIT_SUCCESS;
}
