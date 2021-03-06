#include <api/gpu/fft.h>

#include <api/math.h>
#include <util/error.h>

namespace gpu {
namespace fft {

namespace detail {

#define CLFFT_CHECK(what) ::gpu::fft::detail::clfft_check((what), __FILE__, __LINE__)

const char *get_error_string(clfftStatus error)
{
    switch (error) {
    case CLFFT_BUGCHECK:
        return "CLFFT_BUGCHECK";
    case CLFFT_NOTIMPLEMENTED:
        return "CLFFT_NOTIMPLEMENTED";
    case CLFFT_TRANSPOSED_NOTIMPLEMENTED:
        return "CLFFT_TRANSPOSED_NOTIMPLEMENTED";
    case CLFFT_FILE_NOT_FOUND:
        return "CLFFT_FILE_NOT_FOUND";
    case CLFFT_FILE_CREATE_FAILURE:
        return "CLFFT_FILE_CREATE_FAILURE";
    case CLFFT_VERSION_MISMATCH:
        return "CLFFT_VERSION_MISMATCH";
    case CLFFT_INVALID_PLAN:
        return "CLFFT_INVALID_PLAN";
    case CLFFT_DEVICE_NO_DOUBLE:
        return "CLFFT_DEVICE_NO_DOUBLE";
    case CLFFT_DEVICE_MISMATCH:
        return "CLFFT_DEVICE_MISMATCH";
    default:
        return gpu::compute::get_error_string(error);
    }
}

void clfft_check(clfftStatus status, const char *file, int line)
{
    if (CL_SUCCESS != status) {
        auto msg = get_error_string(status);
        util::die(file, line, "CLFFT error %d: %s\n", status, msg);
    }
}

class fft_api {
public:
    fft_api()
    {
        clfftSetupData setupData;
        CLFFT_CHECK(clfftInitSetupData(&setupData));
        CLFFT_CHECK(clfftSetup(&setupData));
    }

    ~fft_api() { clfftTeardown(); }
};

template <typename T>
struct clfft_precision {
};
template <>
struct clfft_precision<float> {
    static constexpr auto value = CLFFT_SINGLE;
};
template <>
struct clfft_precision<double> {
    static constexpr auto value = CLFFT_DOUBLE;
};

} // namespace detail

ifft2d_hermitian_inplace::ifft2d_hermitian_inplace(
    gpu::compute::command_queue queue,
    const math::ivec2 &size,
    size_t num_batches)
{
    static detail::fft_api fft_api;
    size_t N = size.x;
    size_t M = size.y;
    size_t lenghts[] = { N, M };
    size_t in_stride[] = { 1, N / 2 + 1 };
    size_t out_stride[] = { 1, N + 2 };

    auto context = queue.getInfo<CL_QUEUE_CONTEXT>();
    CLFFT_CHECK(clfftCreateDefaultPlan(&fft_plan, context(), CLFFT_2D, lenghts));
    CLFFT_CHECK(clfftSetPlanBatchSize(fft_plan, num_batches));
    CLFFT_CHECK(clfftSetPlanPrecision(fft_plan, detail::clfft_precision<math::real>::value));
    CLFFT_CHECK(clfftSetResultLocation(fft_plan, CLFFT_INPLACE));
    CLFFT_CHECK(clfftSetLayout(fft_plan, CLFFT_HERMITIAN_INTERLEAVED, CLFFT_REAL));
    CLFFT_CHECK(clfftSetPlanInStride(fft_plan, CLFFT_2D, in_stride));
    CLFFT_CHECK(clfftSetPlanOutStride(fft_plan, CLFFT_2D, out_stride));
    CLFFT_CHECK(clfftSetPlanScale(fft_plan, CLFFT_BACKWARD, cl_float(1)));
    CLFFT_CHECK(clfftSetPlanDistance(fft_plan, M * (N / 2 + 1), M * (N + 2)));

    CLFFT_CHECK(clfftBakePlan(fft_plan, 1, &queue(), nullptr, nullptr));

    size_t tmp_sz;
    CLFFT_CHECK(clfftGetTmpBufSize(fft_plan, &tmp_sz));
    tmp_buf = gpu::compute::buffer(context, CL_MEM_READ_WRITE, tmp_sz);
}

ifft2d_hermitian_inplace::~ifft2d_hermitian_inplace() { clfftDestroyPlan(&fft_plan); }

gpu::compute::event ifft2d_hermitian_inplace::enqueue_transform(
    gpu::compute::command_queue queue,
    gpu::compute::memory_object buffer,
    gpu::compute::event_vector *wait_events)
{
    // cl::Event is just a wrapper around cl_event. In memory they should match
    // exactly.
    const cl_event *cl_wait_events =
        wait_events ? reinterpret_cast<cl_event *>(wait_events->data()) : nullptr;
    cl_uint num_wait_events = wait_events ? static_cast<cl_uint>(wait_events->size()) : 0;

    cl_event res_event;
    CLFFT_CHECK(clfftEnqueueTransform(
        fft_plan, CLFFT_BACKWARD, 1, &queue(), num_wait_events, cl_wait_events, &res_event,
        &buffer(), nullptr, tmp_buf()));
    return cl::Event(res_event);
}

} // namespace fft
} // namespace gpu
