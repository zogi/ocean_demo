#include <ocean/surface_geometry.h>

#include <util/error.h>
#include <util/util.h>

namespace ocean {

#define N_FFT_BATCHES 5

surface_geometry::surface_geometry(gpu::compute::command_queue queue, const surface_params& params)
  : queue(queue),
    wave_spectrum(queue.getInfo<CL_QUEUE_CONTEXT>(), params),
    fft_algorithm(queue, params.fft_size, N_FFT_BATCHES),
    fft_buffer(queue.getInfo<CL_QUEUE_CONTEXT>(), CL_MEM_READ_ONLY, N_FFT_BATCHES * (params.fft_size.x + 2) * params.fft_size.y * sizeof(float)),
    displacement_map(queue.getInfo<CL_QUEUE_CONTEXT>(), params.fft_size, texture_format::TEXTURE_FORMAT_RGBA8),
    height_gradient_map(queue.getInfo<CL_QUEUE_CONTEXT>(), params.fft_size, texture_format::TEXTURE_FORMAT_RG16F)
{
    // Load export kernel.
    auto program = gpu::compute::create_program_from_file(queue.getInfo<CL_QUEUE_CONTEXT>(), "kernels/export_to_texture.cl");
    export_kernel = gpu::compute::kernel(program, "export_to_texture");

    // Set static export kernel parameters.
    export_kernel.setArg(0, fft_buffer);
    export_kernel.setArg(1, params.fft_size.x);
    export_kernel.setArg(2, params.fft_size.y);
    export_kernel.setArg(3, displacement_map.img);
    export_kernel.setArg(4, height_gradient_map.img);
}

surface_geometry::shared_texture::shared_texture(gpu::compute::context& context, math::ivec2 size, texture_format format)
  : tex(size.x, size.y, format)
{
    tex.set_wrap_mode(rendering::texture_2d::WRAP_MODE_REPEAT);
    img = gpu::compute::graphics_image(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, tex.get_api_texture());
    tex.generate_mipmap();
}

void surface_geometry::enqueue_generate(math::real time, const gpu::compute::event_vector *wait_events)
{
    gpu::compute::event event;
    event = wave_spectrum.enqueue_generate(queue, time, fft_buffer, wait_events);
    event = fft_algorithm.enqueue_transform(queue, fft_buffer, &gpu::compute::event_vector({ event }));
    event = enqueue_export_kernel(&gpu::compute::event_vector({ event }));
    event.wait();

    displacement_map.tex.generate_mipmap();
    height_gradient_map.tex.generate_mipmap();
}

gpu::compute::event surface_geometry::enqueue_export_kernel(const gpu::compute::event_vector *wait_events)
{
    gpu::compute::event event;
    std::vector<gpu::compute::memory_object> gl_objects = { displacement_map.img, height_gradient_map.img };

    queue.enqueueAcquireGLObjects(&gl_objects, wait_events, &event);
    gpu::compute::nd_range offset = { 0, 0 }, local_size = { 1, 1 };
    auto size = wave_spectrum.get_params().fft_size;
    gpu::compute::nd_range global_size = { cl::size_type(size.x), cl::size_type(size.y) };
    queue.enqueueNDRangeKernel(export_kernel, offset, global_size, local_size, &gpu::compute::event_vector({ event }), &event);
    queue.enqueueReleaseGLObjects(&gl_objects, &gpu::compute::event_vector({ event }), &event);
    return event;
}

} // namespace ocean
