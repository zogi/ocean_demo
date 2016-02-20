#ifndef __SPECTRUM_H_GUARD
#define __SPECTRUM_H_GUARD

#include <vector>
#include <api/gpu/compute.h>

namespace ocean {

class spectrum {
public:
    spectrum();
    ~spectrum();
    spectrum(const spectrum&) = delete;
    spectrum& operator=(const spectrum&) = delete;

    void set_params(math::real Lx, math::real Lz, int M, int N, math::real A, math::real l, math::real Wx, math::real Wz);
    // The client has to call set_params and before calling bake_params.
    void bake_params(gpu::compute *compute);
    // The client has to call bake before calling enqueue_generate.
    gpu::compute::event enqueue_generate(math::real t, gpu::compute::memory_object out, const gpu::compute::event_vector *wait_events = nullptr);
    int get_N() const { return N; }
    int get_M() const { return M; }
    gpu::compute *get_compute() const { return compute; }

private:
    void set_initial_spectrum();
    void load_phase_shift_kernel();
    math::real phillips_spectrum(int i, int j);

    static constexpr math::real g = math::real(9.80665);

    math::real Lx, Lz;   // x and y dimensions (in meters).
    int N, M;            // Number of samples along dimension x and y respectively.
    math::real A;        // Amplitude multiplier constant.
    math::real l;        // Suppress waves of wavelenght smaller than this (in meters).
    math::real w_mag;    // Wind speed.
    math::real w_x, w_z; // Wind direction.

    gpu::compute *compute;
    gpu::compute::buffer initial_spectrum;
    gpu::compute::kernel phase_shift_kernel;
};

} // namespace ocean

#endif // !__SPECTRUM_H_GUARD
