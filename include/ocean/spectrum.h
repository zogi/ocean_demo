#ifndef __SPECTRUM_H_GUARD
#define __SPECTRUM_H_GUARD

#include <vector>
#include <api/gpu/compute.h>
#include <api/math.h>
#include <ocean/surface_params.h>

namespace ocean {

class spectrum {
public:
    spectrum(gpu::compute::context context, const surface_params& params);
    spectrum(const spectrum&) = delete;
    spectrum& operator=(const spectrum&) = delete;

    gpu::compute::event enqueue_generate(gpu::compute::command_queue queue,
                                         math::real time, gpu::compute::memory_object output_buffer,
                                         const gpu::compute::event_vector *wait_events = nullptr);
    int get_N() const { return params.grid_size.x; }
    int get_M() const { return params.grid_size.y; }

private:
    void set_initial_spectrum(gpu::compute::context context);
    void load_phase_shift_kernel(gpu::compute::context context);
    math::real phillips_spectrum(int i, int j);

    static constexpr math::real g = math::real(9.80665);

    surface_params params;

    gpu::compute::buffer initial_spectrum;
    gpu::compute::kernel phase_shift_kernel;
};

} // namespace ocean

#endif // !__SPECTRUM_H_GUARD
