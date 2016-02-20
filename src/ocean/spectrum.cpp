#include <ocean/spectrum.h>

#include <cassert>
#include <random>
#include <vector>

#include <api/math.h>
#include <util/error.h>
#include <util/util.h>

using math::real;

namespace ocean {

spectrum::spectrum(real Lx, real Lz, int M, int N, real A, real l, real Wx, real Wz)
  : Lx(Lx), Lz(Lz), M(M), N(N), A(A), l(l), compute(nullptr)
{
    w_mag = sqrt(Wx * Wx + Wz * Wz);
    w_x = Wx / w_mag;
    w_z = Wz / w_mag;
}

spectrum::~spectrum()
{
}

void spectrum::bake_params(gpu::compute *compute)
{
    assert(compute);
    this->compute = compute;

    set_initial_spectrum();
    load_phase_shift_kernel();
}

gpu::compute::event spectrum::enqueue_generate(real time, gpu::compute::memory_object out, const gpu::compute::event_vector *wait_events)
{
    phase_shift_kernel.setArg(5, time);
    phase_shift_kernel.setArg(6, out);

    auto offset = gpu::compute::nd_range(0, 0);
    auto global_size = gpu::compute::nd_range(size_t(N) / 2 + 1, size_t(M));
    auto local_size = gpu::compute::nd_range(1, 1);
    gpu::compute::event event;
    compute->get_command_queue().enqueueNDRangeKernel(phase_shift_kernel, offset, global_size, local_size, wait_events, &event);

    return event;
}

void spectrum::set_initial_spectrum()
{
    int size = (N / 2 + 1) * M * 2;
    std::vector<real> data(size);
    std::default_random_engine gen(0);
    std::normal_distribution<real> dist;

    // Generate the 2D Fourier coefficients of the ocean heightfield.
    int idx = 0;
    for (int j = 0; j < M; ++j) {
        for (int i = 0; i <= N / 2; ++i) {
            real p = phillips_spectrum(i, j);
            real mag = sqrt(p * real(0.5));
            data[idx++] = mag * dist(gen); // real part
            data[idx++] = mag * dist(gen); // imaginary part
        }
    }

    // Upload data to GPU.
    int size_bytes = size * sizeof(real);
    initial_spectrum = compute->create_buffer(CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, size_bytes, data.data());
}

void spectrum::load_phase_shift_kernel()
{
    auto program = compute->create_program_from_file("kernels/phase_shift.cl");
    phase_shift_kernel = gpu::compute::kernel(program, "phase_shift");
    phase_shift_kernel.setArg(0, initial_spectrum);
    phase_shift_kernel.setArg(1, Lx);
    phase_shift_kernel.setArg(2, Lz);
    phase_shift_kernel.setArg(3, N);
    phase_shift_kernel.setArg(4, M);
}

real spectrum::phillips_spectrum(int i, int j)
{
    int ii = (i + N / 2) % N - N / 2;
    int jj = (j + M / 2) % M - M / 2;
    real k_x = 2.0f * math::pi * real(ii) / Lx;
    real k_z = 2.0f * math::pi * real(jj) / Lz;
    real k = sqrt(k_x * k_x + k_z * k_z);
    if (k < real(1e-5))
        return real(0);
    real w_cos = (k_x * w_x + k_z * w_z) / k;
    real L = w_mag * w_mag / g;
    real k_sqr = k * k;
    real damp_large = exp(-real(1) / (k_sqr * L * L));
    real damp_small = exp(-k_sqr * l * l);
    return A * damp_large * damp_small * w_cos * w_cos / (k_sqr * k_sqr);
}

} // namespace ocean
