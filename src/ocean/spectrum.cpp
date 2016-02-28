#include <ocean/spectrum.h>

#include <random>
#include <vector>

#include <api/math.h>
#include <util/error.h>
#include <util/util.h>

using math::real;

namespace ocean {

spectrum::spectrum(gpu::compute::context context, const surface_params& params)
  : params(params)
{
    set_initial_spectrum(context);
    load_phase_shift_kernel(context);
}

gpu::compute::event spectrum::enqueue_generate(gpu::compute::command_queue queue,
                                               real time, gpu::compute::memory_object output_buffer,
                                               const gpu::compute::event_vector *wait_events)
{
    phase_shift_kernel.setArg(5, time);
    phase_shift_kernel.setArg(6, output_buffer);

    auto offset = gpu::compute::nd_range(0);
    auto global_size = gpu::compute::nd_range((params.fft_size.x + 2) * params.fft_size.y);
    auto local_size = gpu::compute::nd_range(64);
    gpu::compute::event event;
    queue.enqueueNDRangeKernel(phase_shift_kernel, offset, global_size, local_size, wait_events, &event);

    return event;
}

void spectrum::set_initial_spectrum(gpu::compute::context context)
{
    int elem_count = (params.fft_size.x / 2 + 1) * params.fft_size.y * 2;
    std::vector<real> data(elem_count);
    std::default_random_engine gen(0);
    std::normal_distribution<real> dist;

    // Generate the 2D Fourier coefficients of the ocean heightfield.
    int idx = 0;
    for (int j = 0; j < params.fft_size.y; ++j) {
        for (int i = 0; i <= params.fft_size.x / 2; ++i) {
            real p = phillips_spectrum(i, j);
            real mag = sqrt(p * real(0.5));
            data[idx++] = mag * dist(gen); // real part
            data[idx++] = mag * dist(gen); // imaginary part
        }
    }

    // Upload data to GPU.
    int size_bytes = elem_count * sizeof(real);
    initial_spectrum = gpu::compute::buffer(context, data.begin(), data.end(), true);
}

void spectrum::load_phase_shift_kernel(gpu::compute::context context)
{
    auto program = gpu::compute::create_program_from_file(context, "kernels/phase_shift.cl");
    phase_shift_kernel = gpu::compute::kernel(program, "phase_shift");
    phase_shift_kernel.setArg(0, initial_spectrum);
    phase_shift_kernel.setArg(1, params.tile_size_physical.x);
    phase_shift_kernel.setArg(2, params.tile_size_physical.z);
    phase_shift_kernel.setArg(3, params.fft_size.x);
    phase_shift_kernel.setArg(4, params.fft_size.y);
}

real spectrum::phillips_spectrum(int i, int j)
{
    int N = params.fft_size.x, M = params.fft_size.y;
    int ii = (i + N / 2) % N - N / 2;
    int jj = (j + M / 2) % M - M / 2;
    real k_x = 2.0f * math::pi * real(ii) / params.tile_size_physical.x;
    real k_z = 2.0f * math::pi * real(jj) / params.tile_size_physical.y;
    real k = sqrt(k_x * k_x + k_z * k_z);
    if (k < real(1e-5))
        return real(0);
    real w_cos = (k_x * params.wind_direction.x + k_z * params.wind_direction.y) / k;
    real L = params.wind_speed * params.wind_speed / g;
    real k_sqr = k * k;
    real damp_large = exp(-real(1) / (k_sqr * L * L));
    real damp_small = exp(-k_sqr * params.wavelength_low_threshold * params.wavelength_low_threshold);
    return params.amplitude_factor * damp_large * damp_small * w_cos * w_cos / (k_sqr * k_sqr);
}

} // namespace ocean
