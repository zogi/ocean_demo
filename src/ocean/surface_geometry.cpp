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

surface_geometry::shared_texture::shared_texture(gpu::compute::context context, math::ivec2 size, texture_format format)
  : tex(util::extent(size.x, size.y), format)
{
    tex.set_wrap_mode(rendering::texture_2d::WRAP_MODE_REPEAT);
    tex.set_mag_filter(rendering::texture_2d::MAG_FILTER_LINEAR);
    tex.set_min_filter(rendering::texture_2d::MIN_FILTER_MIPMAP);
    img = gpu::compute::graphics_image(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, tex.get_api_texture());
    tex.generate_mipmap();
}

void surface_geometry::enqueue_generate(math::real time, const gpu::compute::event_vector *wait_events)
{
    gpu::compute::event event_spectrum, event_fft, event_export;
    event_spectrum = wave_spectrum.enqueue_generate(queue, time, fft_buffer, wait_events);
    auto event_vector_spectrum = gpu::compute::event_vector({ event_spectrum });
    event_fft = fft_algorithm.enqueue_transform(queue, fft_buffer, &event_vector_spectrum);
    auto event_vector_export = gpu::compute::event_vector({ event_fft });
    event_export = enqueue_export_kernel(&event_vector_export);
    event_export.wait();

    int64_t spectrum_start_ns = event_spectrum.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    int64_t spectrum_end_ns = event_spectrum.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    timings.phase_shift_milliseconds = (spectrum_end_ns - spectrum_start_ns) * 1e-6;

    int64_t fft_start_ns = event_fft.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    int64_t fft_end_ns = event_fft.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    timings.fft_milliseconds = (fft_end_ns - fft_start_ns) * 1e-6;

    int64_t export_start_ns = event_export.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    int64_t export_end_ns = event_export.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    timings.export_milliseconds = (export_end_ns - export_start_ns) * 1e-6;

    {
    auto mipmap_timer = util::scoped_timer(timer, timings.mipmap_generation_milliseconds);
    displacement_map.tex.generate_mipmap();
    height_gradient_map.tex.generate_mipmap();
    }
}

gpu::compute::event surface_geometry::enqueue_export_kernel(const gpu::compute::event_vector *wait_events)
{
    gpu::compute::event event;
    std::vector<gpu::compute::memory_object> gl_objects = { displacement_map.img, height_gradient_map.img };

    queue.enqueueAcquireGLObjects(&gl_objects, wait_events, &event);
    gpu::compute::nd_range offset = { 0, 0 }, local_size = { 1, 1 };
    auto size = wave_spectrum.get_params().fft_size;
    gpu::compute::nd_range global_size = { cl::size_type(size.x), cl::size_type(size.y) };
    auto event_vector_acquire = gpu::compute::event_vector({ event });
    queue.enqueueNDRangeKernel(export_kernel, offset, global_size, local_size, &event_vector_acquire, &event);
    auto event_vector_kernel = gpu::compute::event_vector({ event });
    queue.enqueueReleaseGLObjects(&gl_objects, &event_vector_kernel, &event);
    return event;
}

} // namespace ocean
