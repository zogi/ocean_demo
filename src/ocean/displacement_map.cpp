#include <ocean/displacement_map.h>

#include <cassert>

#include <util/error.h>
#include <util/util.h>

namespace ocean {

displacement_map::displacement_map(gpu::compute::command_queue queue, const surface_params& params)
  : wave_spectrum(queue.getInfo<CL_QUEUE_CONTEXT>(), params)
{
    this->queue = queue;

    int N = wave_spectrum.get_N();
    int M = wave_spectrum.get_M();
    auto context = queue.getInfo<CL_QUEUE_CONTEXT>();

    const size_t n_fft_batches = 5;
    fft_algorithm.create_plan(queue, N, M, n_fft_batches);
    size_t buf_size_bytes = n_fft_batches * (N + 2) * M * sizeof(float);
    fft_buffer = gpu::compute::buffer(context, CL_MEM_READ_WRITE, buf_size_bytes);

    displacement.init(context, N, M, texture_format::TEXTURE_FORMAT_RGBA8);
    height_gradient.init(context, N, M, texture_format::TEXTURE_FORMAT_RG16F);

    load_export_kernel();
}

void displacement_map::shared_texture::init(gpu::compute::context context, size_t width, size_t height, texture_format format)
{
    tex.init(width, height, format);
    tex.set_max_anisotropy(2);
    img = gpu::compute::graphics_image(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, tex.get_api_texture());
    tex.generate_mipmap();
}

void displacement_map::enqueue_generate(math::real time, const gpu::compute::event_vector *wait_events)
{
    gpu::compute::event event;
    event = wave_spectrum.enqueue_generate(queue, time, fft_buffer, wait_events);
    event = fft_algorithm.enqueue_transform(queue, fft_buffer, &gpu::compute::event_vector({ event }));
    event = enqueue_export_kernel(&gpu::compute::event_vector({ event }));
    event.wait();

    displacement.tex.generate_mipmap();
    height_gradient.tex.generate_mipmap();
}

void displacement_map::load_export_kernel()
{
    auto program = gpu::compute::create_program_from_file(queue.getInfo<CL_QUEUE_CONTEXT>(), "kernels/export_to_texture.cl");
    export_kernel = gpu::compute::kernel(program, "export_to_texture");

    export_kernel.setArg(0, fft_buffer);
    export_kernel.setArg(1, wave_spectrum.get_N());
    export_kernel.setArg(2, wave_spectrum.get_M());
    export_kernel.setArg(3, displacement.img);
    export_kernel.setArg(4, height_gradient.img);
}

gpu::compute::event displacement_map::enqueue_export_kernel(const gpu::compute::event_vector *wait_events)
{
    gpu::compute::event event;
    std::vector<gpu::compute::memory_object> gl_objects = { displacement.img, height_gradient.img };

    queue.enqueueAcquireGLObjects(&gl_objects, wait_events, &event);
    gpu::compute::nd_range offset = { 0, 0 }, local_size = { 1, 1 };
    gpu::compute::nd_range global_size = { static_cast<size_t>(wave_spectrum.get_N()), static_cast<size_t>(wave_spectrum.get_M()) };
    queue.enqueueNDRangeKernel(export_kernel, offset, global_size, local_size, &gpu::compute::event_vector({ event }), &event);
    queue.enqueueReleaseGLObjects(&gl_objects, &gpu::compute::event_vector({ event }), &event);
    return event;
}

} // namespace ocean
