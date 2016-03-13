#include <api/gpu/graphics.h>

#include <cstdio>
#include <cstring>

#include <GL/glu.h>

#include <util/error.h>
#include <util/log.h>

namespace gpu {
namespace graphics {

namespace {

bool is_extension_supported(const char *required_extension)
{
    GLint num_extensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
    for (int i = 0; i < num_extensions; ++i) {
        const char *extension = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i));
        if (strncmp(required_extension, extension, 1024) == 0) {
            return true;
        }
    }
    return false;
}

} // unnamed namespace

void init()
{
    if (gl3wInit()) {
        DIE("failed to initialize OpenGL\n");
    }
    if (!gl3wIsSupported(OPENGL_MAJOR, OPENGL_MINOR)) {
        DIE("OpenGL %d.%d not supported\n", OPENGL_MAJOR, OPENGL_MINOR);
    }

    // Check if GL_EXT_texture_filter_anisotropic extension is supported.
    if (!is_extension_supported("GL_EXT_texture_filter_anisotropic")) {
        DIE("GL_EXT_texture_filter_anisotropic extension is not supported.");
    }

    LOG("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
