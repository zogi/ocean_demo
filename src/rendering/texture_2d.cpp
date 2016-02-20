#include <rendering/texture_2d.h>

#include <unordered_map>

namespace rendering {

namespace {

struct texture_format_traits {
    GLint internalformat;
    GLenum format;
    GLenum type;
};

std::unordered_map<texture_2d::texture_format, texture_format_traits> format_traits = {
    { texture_2d::TEXTURE_FORMAT_RGBA8, {GL_RGBA8, GL_RGBA, GL_BYTE}},     // TEXTURE_FORMAT_RGBA8
    { texture_2d::TEXTURE_FORMAT_RG16F, {GL_RG16F, GL_RG, GL_HALF_FLOAT}}, // TEXTURE_FORMAT_RG16F
};

} // unnamed namespace

void texture_2d::init(size_t width, size_t height, texture_format format)
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    auto traits = format_traits[format];
    glTexImage2D(GL_TEXTURE_2D, 0, format, GLsizei(width), GLsizei(height), 0, traits.format, traits.type, nullptr);
    set_mag_filter(MAG_FILTER_LINEAR);
    set_min_filter(MIN_FILTER_MIPMAP);
    set_wrap_mode(WRAP_MODE_REPEAT);
}

} // namespace rendering
