#ifndef __FFT_H_GUARD
#define __FFT_H_GUARD

#include <api/gpu/compute.h>
#include <api/math.h>
#include <clFFT.h>

namespace gpu {
namespace fft {

namespace api {
typedef clfftPlanHandle plan_handle;
} // namespace api

class ifft2d_hermitian_inplace {
public:
    ifft2d_hermitian_inplace(gpu::compute::command_queue queue, const math::ivec2 &size, size_t num_batches);
    ~ifft2d_hermitian_inplace();
    ifft2d_hermitian_inplace(const ifft2d_hermitian_inplace &) = delete;
    ifft2d_hermitian_inplace &operator=(const ifft2d_hermitian_inplace &) = delete;

    gpu::compute::event enqueue_transform(
        gpu::compute::command_queue queue,
        gpu::compute::memory_object buffer,
        gpu::compute::event_vector *wait_events = nullptr);

private:
    api::plan_handle fft_plan;
    gpu::compute::memory_object tmp_buf;
};

} // namespace fft
} // namespace gpu

#endif // !__FFT_H_GUARD
