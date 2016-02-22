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
namespace compute {

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

    command_queue init(os::window::graphics_context graphics_context);
    program create_program_from_file(gpu::compute::context context, const char *file_name);
    const char *get_error_string(cl_int status);

} // namespace compute
} // namespace gpu

#endif // !__COMPUTE_H_GUARD
