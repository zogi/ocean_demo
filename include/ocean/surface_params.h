#ifndef __SURFACE_PARAMS_H_GUARD
#define __SURFACE_PARAMS_H_GUARD

#include <api/math.h>

namespace ocean {

struct surface_params {
    math::ivec2 fft_size;                // Number of samples along the horizontal dimensions.
    math::vec3 tile_size_physical;       // In meters.
    math::vec3 tile_size_logical;        // In rendering units.
    math::real amplitude;
    math::real wavelength_low_threshold; // Suppress waves of wavelenght smaller than this (in meters).
    math::vec2 wind_direction;
    math::real wind_speed;
    void set_wind_vector(const math::vec2 wind_vector)
    {
        wind_speed = math::length(wind_vector);
        wind_direction = wind_vector / wind_speed;
    }
};

} // namespace ocean

#endif // !__SURFACE_PARAMS_H_GUARD
