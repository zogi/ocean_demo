#include <rendering/shader_effect.h>

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <util/error.h>
#include <util/log.h>
#include <util/util.h>

#include <api/gpu/graphics.h>

namespace rendering {

namespace {

static const GLenum gl_shader_types[] = { GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER,
                                          GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER,
                                          GL_FRAGMENT_SHADER };

static const std::unordered_map<GLenum, const char *> gl_shader_type_defines =
    { { GL_VERTEX_SHADER, "#define VERTEX_SHADER\n" },
      { GL_TESS_CONTROL_SHADER, "#define TESSELLATION_CONTROL_SHADER\n" },
      { GL_TESS_EVALUATION_SHADER, "#define TESSELLATION_EVALUATION_SHADER\n" },
      { GL_GEOMETRY_SHADER, "#define GEOMETRY_SHADER\n" },
      { GL_FRAGMENT_SHADER, "#define FRAGMENT_SHADER\n" } };

const char *get_shader_type_define(GLenum shader_type)
{
    return gl_shader_type_defines.at(shader_type);
}

static const std::unordered_map<GLenum, const char *> gl_shader_type_names =
    { { GL_VERTEX_SHADER, "vertex shader" },
      { GL_TESS_CONTROL_SHADER, "tessellation control shader" },
      { GL_TESS_EVALUATION_SHADER, "tessellation evaluation shader" },
      { GL_GEOMETRY_SHADER, "geometry shader" },
      { GL_FRAGMENT_SHADER, "fragment shader" } };

const char *get_shader_type_name(GLenum shader_type)
{
    return gl_shader_type_names.at(shader_type);
}

} // unnamed namespace

shader_effect::~shader_effect() { glDeleteProgram(program_id); }

void shader_effect::load_shaders(const char *filename, shader_type_set pipeline_stages)
{
    // Load specified shaders from file and compile them.
    constexpr int num_shader_stages = 5;
    std::vector<GLuint> shaders;
    shaders.reserve(num_shader_stages);
    for (int i = 0; i < num_shader_stages; ++i)
        if (pipeline_stages & (1 << i))
            shaders.push_back(compile_gl_shader(gl_shader_types[i], filename));

    // Link the program
    LOG("GL: Linking program.\n");
    GLuint program_id = glCreateProgram();
    for (auto shader : shaders) {
        glAttachShader(program_id, shader);
        GL_CHECK();
    }
    glLinkProgram(program_id);

    // Check for link errors and show them if found
    GLint result = GL_FALSE;
    int info_log_lenght;
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_lenght);
    std::vector<char> error_message(std::max(info_log_lenght, int(1)));
    glGetProgramInfoLog(program_id, info_log_lenght, nullptr, error_message.data());
    if (!error_message.empty() && error_message[0]) {
        LOG("%s\n", error_message.data());
    }
    GL_CHECK();

    for (auto shader : shaders)
        glDeleteShader(shader);

    this->program_id = program_id;
}

void shader_effect::use() const
{
    glUseProgram(program_id);
    GL_CHECK();
    if (z_test_enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    GL_CHECK();
    glDepthMask(z_write_enabled ? GL_TRUE : GL_FALSE);
    GL_CHECK();
}

GLint shader_effect::get_param_location(const char *param_name) const
{
    GLint loc = glGetUniformLocation(program_id, param_name);
    if (loc < 0) {
        LOG("WARNING: wrong program parameter name: '%s'\n", param_name);
    }
    return loc;
}

GLuint shader_effect::compile_gl_shader(GLenum shader_type, const char *filename, const char *defines)
{
    GLuint shader_id = glCreateShader(shader_type);
    auto shader_code = util::read_file_contents(filename);

    LOG("GL: Compiling %s: %s\n", get_shader_type_name(shader_type), filename);
    char const *shader_code_c_str = shader_code.c_str();
    const char *code[] = { "#version " GLSL_VERSION_STRING " core\n",
                           get_shader_type_define(shader_type), defines, shader_code_c_str };
    glShaderSource(shader_id, 4, code, nullptr);
    glCompileShader(shader_id);

    GLint result = GL_FALSE;
    int info_log_lenght;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_lenght);
    std::vector<char> error_message(info_log_lenght);
    glGetShaderInfoLog(shader_id, info_log_lenght, nullptr, error_message.data());
    if (!error_message.empty()) {
        LOG("%s\n", error_message.data());
        abort();
    }
    GL_CHECK();

    return shader_id;
}

} // namespace rendering
