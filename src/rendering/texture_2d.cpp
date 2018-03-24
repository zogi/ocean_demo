#include <rendering/texture_2d.h>

#include <map>

namespace rendering {

namespace {

struct texture_format_traits {
    GLint internalformat;
    GLenum format;
    GLenum type;
};

std::map<texture_2d::texture_format, texture_format_traits> format_traits =
    { { texture_2d::TEXTURE_FORMAT_RGBA8, { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE } },
      { texture_2d::TEXTURE_FORMAT_RG16F, { GL_RG16F, GL_RG, GL_HALF_FLOAT } },
      { texture_2d::TEXTURE_FORMAT_R8I, { GL_R8I, GL_RED_INTEGER, GL_UNSIGNED_BYTE } } };

} // unnamed namespace

texture_2d::texture_2d(const util::extent &extent, texture_format format, const void *data)
{
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, tex);
    auto traits = format_traits[format];
    glTexImage2D(
        GL_TEXTURE_2D, 0, traits.internalformat, GLsizei(extent.width), GLsizei(extent.height), 0,
        traits.format, traits.type, data);
    GL_CHECK();
    glActiveTexture(GL_TEXTURE0);
}

} // namespace rendering
