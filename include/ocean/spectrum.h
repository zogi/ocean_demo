#ifndef __SPECTRUM_H_GUARD
#define __SPECTRUM_H_GUARD

#include <vector>
#include <api/gpu/compute.h>
#include <ocean/surface_params.h>

namespace ocean {

class spectrum {
public:
    spectrum(const surface_params& params);
    ~spectrum();
    spectrum(const spectrum&) = delete;
    spectrum& operator=(const spectrum&) = delete;

    void bake_params(gpu::compute *compute);
    // The client has to call bake before calling enqueue_generate.
    gpu::compute::event enqueue_generate(math::real time, gpu::compute::memory_object out, const gpu::compute::event_vector *wait_events = nullptr);
    int get_N() const { return params.grid_size.x; }
    int get_M() const { return params.grid_size.y; }
    gpu::compute *get_compute() const { return compute; }

private:
    void set_initial_spectrum();
    void load_phase_shift_kernel();
    math::real phillips_spectrum(int i, int j);

    static constexpr math::real g = math::real(9.80665);

    surface_params params;

    gpu::compute *compute;
    gpu::compute::buffer initial_spectrum;
    gpu::compute::kernel phase_shift_kernel;
};

} // namespace ocean

#endif // !__SPECTRUM_H_GUARD
