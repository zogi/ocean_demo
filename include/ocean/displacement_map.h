#ifndef __DISPLACEMENT_MAP_H_GUARD
#define __DISPLACEMENT_MAP_H_GUARD

#include <api/gpu/compute.h>
#include <api/gpu/fft.h>
#include <api/math.h>
#include <ocean/spectrum.h>
#include <rendering/texture_2d.h>

namespace ocean {

class displacement_map {
public:

    displacement_map() : wave_spectrum(nullptr), compute(nullptr) {}
    ~displacement_map();
    displacement_map(const displacement_map&) = delete;
    displacement_map& operator=(const displacement_map&) = delete;

    void set_spectrum(spectrum *wave_spectrum);
    void enqueue_generate(math::real time, const gpu::compute::event_vector *wait_events = nullptr);
    void bind_textures(gpu::graphics::texture_unit displacement_tex_unit, gpu::graphics::texture_unit height_gradient_tex_unit);

private:
    typedef rendering::texture_2d::texture_format texture_format;

    struct shared_texture {
        void init(gpu::compute *compute, size_t width, size_t height, texture_format format);
        gpu::compute::graphics_image img;
        rendering::texture_2d tex;
    };

    void load_export_kernel();
    gpu::compute::event enqueue_export_kernel(const gpu::compute::event_vector *wait_events = nullptr);

    spectrum *wave_spectrum;
    gpu::compute *compute;
    gpu::fft::ifft2d_hermitian_inplace fft_algorithm;
    gpu::compute::memory_object fft_buffer;
    gpu::compute::kernel export_kernel;
    shared_texture displacement, height_gradient;
};

inline void displacement_map::bind_textures(gpu::graphics::texture_unit displacement_tex_unit, gpu::graphics::texture_unit height_gradient_tex_unit)
{
    displacement.tex.bind(displacement_tex_unit);
    height_gradient.tex.bind(height_gradient_tex_unit);
}

} // namespace ocean

#endif // !__DISPLACEMENT_MAP_H_GUARD
