#ifndef __COMPUTE_H_GUARD
#define __COMPUTE_H_GUARD

#include <vector>

#include <api/os/window.h>
#include <api/gpu/graphics.h>

#define CL_HPP_ENABLE_EXCEPTIONS 1
#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL/cl2.hpp>
#include <CL/cl_gl.h>

namespace gpu {

class compute {
public:
    typedef cl::Platform       platform;
    typedef cl::Device         device;
    typedef cl::Context        context;
    typedef cl::CommandQueue   command_queue;
    typedef cl::Memory         memory_object;
    typedef cl::Buffer         buffer;
    typedef cl::ImageGL        graphics_image;
    typedef cl::Program        program;
    typedef cl::Kernel         kernel;
    typedef cl::Event          event;
    typedef std::vector<event> event_vector;
    typedef cl::NDRange        nd_range;

    compute(os::window::graphics_context graphics_context);

    buffer create_buffer(cl_mem_flags mem_flags, size_t size, void *host_ptr = nullptr);
    graphics_image create_graphics_image(cl_mem_flags mem_flags, graphics::texture texture);
    static program create_program_from_file(gpu::compute::context context, const char *file_name);

    context& get_context() { return c_context; }
    command_queue& get_command_queue() { return c_queue; }

    static const char *get_error_string(cl_int status);

private:
    platform c_platform;
    device c_device;
    context c_context;
    command_queue c_queue;
};

inline compute::buffer compute::create_buffer(cl_mem_flags mem_flags, size_t size, void *host_ptr)
{
    return buffer(c_context, mem_flags, size, host_ptr);
}

inline compute::graphics_image compute::create_graphics_image(cl_mem_flags mem_flags, graphics::texture texture)
{
    return graphics_image(c_context, mem_flags, GL_TEXTURE_2D, 0, texture);
}

} // namespace gpu

#endif // !__COMPUTE_H_GUARD
