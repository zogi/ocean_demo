#ifndef __SURFACE_GEOMETRY_H_GUARD
#define __SURFACE_GEOMETRY_H_GUARD

#include <api/gpu/compute.h>
#include <api/gpu/fft.h>
#include <api/math.h>
#include <ocean/surface_params.h>
#include <ocean/spectrum.h>
#include <rendering/texture_2d.h>

namespace ocean {

class surface_geometry {
public:

    surface_geometry(gpu::compute::command_queue queue, const surface_params& params);
    surface_geometry(const surface_geometry&) = delete;
    surface_geometry& operator=(const surface_geometry&) = delete;

    // Generates displacement map and height gradient map to textures.
    void enqueue_generate(math::real time, const gpu::compute::event_vector *wait_events = nullptr);
    void bind_displacement_texture(gpu::graphics::texture_unit tex_unit) { displacement_map.tex.bind(tex_unit); }
    void bind_height_gradient_texture(gpu::graphics::texture_unit tex_unit) { height_gradient_map.tex.bind(tex_unit); }

private:
    typedef rendering::texture_2d::texture_format texture_format;

    struct shared_texture {
        shared_texture(gpu::compute::context& context, math::ivec2 size, texture_format format);
        gpu::compute::graphics_image img;
        rendering::texture_2d tex;
    };

    gpu::compute::event enqueue_export_kernel(const gpu::compute::event_vector *wait_events = nullptr);

    gpu::compute::command_queue queue;
    spectrum wave_spectrum;
    gpu::fft::ifft2d_hermitian_inplace fft_algorithm;
    gpu::compute::buffer fft_buffer;
    gpu::compute::kernel export_kernel;
    shared_texture displacement_map, height_gradient_map;
};

} // namespace ocean

#endif // !__SURFACE_GEOMETRY_H_GUARD