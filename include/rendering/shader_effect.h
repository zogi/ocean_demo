#ifndef __SHADER_EFFECT_H_GUARD
#define __SHADER_EFFECT_H_GUARD

#include <api/gpu/graphics.h>
#include <api/math.h>
#include <util/rect.h>

namespace rendering {

namespace shader_type {
enum shader_type_set { VERTEX = 1, TESS_CONTROL = 2, TESS_EVAL = 4, GEOMETRY = 8, FRAGMENT = 16 };

constexpr shader_type_set operator|(shader_type_set a, shader_type_set b)
{
    return static_cast<shader_type_set>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr shader_type_set TESSELLATION = TESS_CONTROL | TESS_EVAL;
} // namespace shader_type

typedef shader_type::shader_type_set shader_type_set;

class shader_effect {
public:
    shader_effect() : program_id(0), z_test_enabled(true), z_write_enabled(true) {}
    ~shader_effect();
    shader_effect(const shader_effect &) = delete;
    shader_effect &operator=(const shader_effect &) = delete;

    template <typename T>
    void set_parameter(const char *param_name, const T &value) const;
    void load_shaders(const char *filename, shader_type_set pipeline_stages);
    bool get_z_test_state() const { return z_test_enabled; }
    void set_z_test_state(bool enabled) { z_test_enabled = enabled; }
    bool get_z_write_state() const { return z_write_enabled; }
    void set_z_write_state(bool enabled) { z_write_enabled = enabled; }
    void use() const;

private:
    GLint get_param_location(const char *param_name) const;
    GLuint compile_gl_shader(GLenum shader_type, const char *filename, const char *defines = "");

    GLuint program_id;
    bool z_test_enabled, z_write_enabled;
};

template <typename T>
inline void shader_effect::set_parameter(const char *param_name, const T &value) const
{
    static_assert(sizeof(T) == -1, "not implemented");
}

template <>
inline void shader_effect::set_parameter<GLint>(const char *param_name, const GLint &value) const
{
    glUniform1i(get_param_location(param_name), value);
    GL_CHECK();
}

template <>
inline void shader_effect::set_parameter<GLuint>(const char *param_name, const GLuint &value) const
{
    glUniform1i(get_param_location(param_name), value);
    GL_CHECK();
}

template <>
inline void shader_effect::set_parameter<GLfloat>(const char *param_name, const GLfloat &value) const
{
    glUniform1f(get_param_location(param_name), value);
    GL_CHECK();
}

template <>
inline void
shader_effect::set_parameter<math::ivec2>(const char *param_name, const math::ivec2 &value) const
{
    glUniform2i(get_param_location(param_name), value.x, value.y);
    GL_CHECK();
}
template <>
inline void
shader_effect::set_parameter<math::ivec3>(const char *param_name, const math::ivec3 &value) const
{
    glUniform3i(get_param_location(param_name), value.x, value.y, value.z);
    GL_CHECK();
}
template <>
inline void
shader_effect::set_parameter<math::ivec4>(const char *param_name, const math::ivec4 &value) const
{
    glUniform4i(get_param_location(param_name), value.x, value.y, value.z, value.w);
    GL_CHECK();
}

template <>
inline void shader_effect::set_parameter<math::vec2>(const char *param_name, const math::vec2 &value) const
{
    glUniform2f(get_param_location(param_name), value.x, value.y);
    GL_CHECK();
}
template <>
inline void shader_effect::set_parameter<math::vec3>(const char *param_name, const math::vec3 &value) const
{
    glUniform3f(get_param_location(param_name), value.x, value.y, value.z);
    GL_CHECK();
}
template <>
inline void shader_effect::set_parameter<math::vec4>(const char *param_name, const math::vec4 &value) const
{
    glUniform4f(get_param_location(param_name), value.x, value.y, value.z, value.w);
    GL_CHECK();
}

template <>
inline void shader_effect::set_parameter<math::mat3>(const char *param_name, const math::mat3 &value) const
{
    glUniformMatrix3fv(get_param_location(param_name), 1, GL_FALSE, math::value_ptr(value));
    GL_CHECK();
}
template <>
inline void shader_effect::set_parameter<math::mat4>(const char *param_name, const math::mat4 &value) const
{
    glUniformMatrix4fv(get_param_location(param_name), 1, GL_FALSE, math::value_ptr(value));
    GL_CHECK();
}

template <>
inline void
shader_effect::set_parameter<util::offset>(const char *param_name, const util::offset &value) const
{
    glUniform2i(get_param_location(param_name), value.x, value.y);
    GL_CHECK();
}
template <>
inline void
shader_effect::set_parameter<util::extent>(const char *param_name, const util::extent &value) const
{
    glUniform2i(get_param_location(param_name), value.width, value.height);
    GL_CHECK();
}
} // namespace rendering

#endif // !__SHADER_EFFECT_H_GUARD
