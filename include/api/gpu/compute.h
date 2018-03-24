#ifndef __COMPUTE_H_GUARD
#define __COMPUTE_H_GUARD

#include <set>
#include <vector>

#include <api/gpu/graphics.h>
#include <api/os/window.h>

#define CL_HPP_ENABLE_EXCEPTIONS 1
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include <CL/cl2.hpp>
#include <CL/cl_gl.h>

namespace gpu {
namespace compute {

typedef cl::Platform platform;
typedef cl::Device device;
typedef cl::Context context;
typedef cl::CommandQueue command_queue;
typedef cl::Memory memory_object;
typedef cl::Buffer buffer;
typedef cl::ImageGL graphics_image;
typedef cl::Program program;
typedef cl::Kernel kernel;
typedef cl::Event event;
typedef std::vector<event> event_vector;
typedef cl::NDRange nd_range;

command_queue init(const os::window &window);
program create_program_from_file(gpu::compute::context context, const char *file_name);
const char *get_error_string(cl_int status);

class extension_set : public std::set<std::string> {
public:
    typedef value_type extension;

    extension_set(const std::string &extensions = "");
    extension_set(device compute_device);
    extension_set(std::initializer_list<std::string> l) : std::set<std::string>(l) {}

    void add_extensions(const std::string &extensions);
    bool has_extension(const std::string &extension) { return count(extension) != 0; }
    bool is_subset_of(const extension_set &superset) const;
    std::string to_string() const;
};

} // namespace compute
} // namespace gpu

#endif // !__COMPUTE_H_GUARD
