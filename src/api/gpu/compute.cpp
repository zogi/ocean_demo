#include <api/gpu/compute.h>

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <util/error.h>
#include <util/log.h>
#include <util/util.h>

namespace gpu {
namespace compute {

namespace {
    using std::string;
    using std::stringstream;
    using std::vector;

    typedef string extension;
    typedef vector<extension> extension_vector;

    void append_extensions(const extension& in, extension_vector& out)
    {
        typedef std::istream_iterator<extension> iter;
        auto ss = stringstream(in);
        std::copy(iter(ss), iter(), std::back_inserter(out));
    }

    bool check_extension_requirements(const extension_vector& required, const extension_vector& supported)
    {
        return std::all_of(required.begin(), required.end(), [&supported](const extension& extension) {
            return std::find(supported.begin(), supported.end(), extension) != supported.end(); });
    }

    compute::device get_device(cl_device_type device_type, const extension_vector& required_extensions)
    {
        vector<compute::platform> platforms;
        compute::platform::get(&platforms);
        for (auto& platform : platforms) {
            extension_vector platform_extensions;
            append_extensions(platform.getInfo<CL_PLATFORM_EXTENSIONS>(), platform_extensions);

            std::vector<compute::device> devices;
            platform.getDevices(device_type, &devices);
            for (const auto& device : devices) {
                extension_vector device_extensions = platform_extensions;
                append_extensions(device.getInfo<CL_DEVICE_EXTENSIONS>(), device_extensions);

                if (check_extension_requirements(required_extensions, device_extensions))
                    return device;
            }
        }

        DIE("No suitable compute device found.");
        return compute::device();
    }

} // unnamed namespace

command_queue init(os::window::graphics_context graphics_context)
{
    extension_vector required_extensions = { "cl_khr_gl_sharing", "cl_khr_gl_event" };

    auto c_device = get_device(CL_DEVICE_TYPE_GPU, required_extensions);
    auto c_platform_id = c_device.getInfo<CL_DEVICE_PLATFORM>();

    cl_context_properties context_properties[] = {
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(c_platform_id),
        CL_GL_CONTEXT_KHR, reinterpret_cast<cl_context_properties>(graphics_context),
        0 };
    auto c_context = context(c_device, context_properties, nullptr, nullptr);

    return command_queue(c_context, c_device, cl::QueueProperties::Profiling);
}

program create_program_from_file(context c_context, const char *file_name)
{
    auto source = util::read_file_contents(file_name);
    auto res = program(c_context, source, false);
    auto devices = c_context.getInfo<CL_CONTEXT_DEVICES>();

    try {
        res.build(devices);
    } catch (cl::Error e) {
        for (auto device : devices) {
            auto device_name = device.getInfo<CL_DEVICE_NAME>();
            auto build_log = res.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
            LOG("%s: %s\n", device_name.c_str(), build_log.c_str());
        }
        throw;
    }

    return res;
}

const char *get_error_string(cl_int status)
{
    switch (status) {
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown error";
    }
}

} // namespace compute
} // namespace gpu
