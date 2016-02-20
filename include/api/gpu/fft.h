#ifndef __FFT_H_GUARD
#define __FFT_H_GUARD

#include <clFFT.h>
#include <api/gpu/compute.h>

namespace gpu {
namespace fft {

namespace api {
typedef clfftPlanHandle plan_handle;
} // namespace api

class ifft2d_hermitian_inplace {
public:
    ifft2d_hermitian_inplace();
    ~ifft2d_hermitian_inplace();
    ifft2d_hermitian_inplace(const ifft2d_hermitian_inplace&) = delete;
    ifft2d_hermitian_inplace& operator=(const ifft2d_hermitian_inplace&) = delete;

    void create_plan(size_t N, size_t M, size_t num_batches, gpu::compute *compute);
    gpu::compute::event enqueue_transform(gpu::compute::memory_object buffer, gpu::compute::event_vector *wait_events = nullptr);

private:
    void destroy_plan();

    api::plan_handle fft_plan;
    gpu::compute *compute;
    gpu::compute::memory_object tmp_buf;
};

} // namespace fft
} // namespace gpu

#endif // !__FFT_H_GUARD
