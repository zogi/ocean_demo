#include <api/gpu/graphics.h>

#include <unordered_map>

#include <GL/GLU.h>

#include <util/error.h>
#include <util/log.h>

namespace gpu {
namespace graphics {

void init()
{
    if (gl3wInit()) {
        DIE("failed to initialize OpenGL\n");
    }
    if (!gl3wIsSupported(OPENGL_MAJOR, OPENGL_MINOR)) {
        DIE("OpenGL %d.%d not supported\n", OPENGL_MAJOR, OPENGL_MINOR);
    }
    LOG("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_MULTISAMPLE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void check_last_error(const char *file, int line)
{
    GLenum status = glGetError();
    if (status != GL_NO_ERROR) {
        util::die(file, line, "GL error %d: %s\n", status, gluErrorString(status));
    }
}

} // namespace graphics
} // namespace gpu
