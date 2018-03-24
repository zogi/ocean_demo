#ifndef __GRAPHICS_H_GUARD
#define __GRAPHICS_H_GUARD

#define NOMINMAX
#include <GL/gl3w.h>
#include <GL/glext.h>
#undef NOMINMAX

namespace gpu {
namespace graphics {

#define OPENGL_MAJOR 4
#define OPENGL_MINOR 3
#define GLSL_VERSION_STRING "430"

typedef GLuint texture;
typedef GLuint framebuffer;
typedef GLuint renderbuffer;
typedef GLuint texture_unit;

void init();
inline void set_viewport_size(int width, int height)
{
    glViewport(0, 0, GLsizei(width), GLsizei(height));
}

#define GL_CHECK() ::gpu::graphics::check_last_error(__FILE__, __LINE__)
void check_last_error(const char *file, int line);

} // namespace graphics
} // namespace gpu

#endif // !__GRAPHICS_H_GUARD
