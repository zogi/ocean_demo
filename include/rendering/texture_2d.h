#ifndef __TEXTURE_2D_H_GUARD
#define __TEXTURE_2D_H_GUARD

#include <api/gpu/graphics.h>
#include <util/rect.h>

namespace rendering {

class texture_2d {
public:
    enum mag_filter : GLenum { MAG_FILTER_NEAREST = GL_NEAREST, MAG_FILTER_LINEAR = GL_LINEAR };
    enum min_filter : GLenum {
        MIN_FILTER_NEAREST = GL_NEAREST,
        MIN_FILTER_LINEAR = GL_LINEAR,
        MIN_FILTER_MIPMAP = GL_LINEAR_MIPMAP_LINEAR
    };
    enum wrap_mode : GLenum {
        WRAP_MODE_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        WRAP_MODE_REPEAT = GL_REPEAT
    };
    enum texture_format : GLenum {
        TEXTURE_FORMAT_RGBA8 = GL_RGBA8,
        TEXTURE_FORMAT_RG16F = GL_RG16F,
        TEXTURE_FORMAT_R8I = GL_R8I
    };

    texture_2d(const util::extent &extent, texture_format format, const void *data = nullptr);
    ~texture_2d() { glDeleteTextures(1, &tex); }
    texture_2d(const texture_2d &) = delete;
    texture_2d(texture_2d &&texture) : tex(texture.tex) { texture.tex = 0; }
    texture_2d &operator=(const texture_2d &) = delete;
    texture_2d &operator=(texture_2d &&texture)
    {
        tex = texture.tex;
        texture.tex = 0;
        return *this;
    }

    void set_mag_filter(mag_filter mag_filter)
    {
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, mag_filter);
        GL_CHECK();
    }
    void set_min_filter(min_filter min_filter)
    {
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, min_filter);
        GL_CHECK();
    }
    void set_wrap_mode(wrap_mode wrap_mode);
    void set_max_anisotropy(GLfloat max_anisotropy = 0.0);
    void generate_mipmap()
    {
        glGenerateTextureMipmap(tex);
        GL_CHECK();
    }
    void bind(gpu::graphics::texture_unit tex_unit)
    {
        glBindTextureUnit(tex_unit, tex);
        GL_CHECK();
    }
    gpu::graphics::texture get_api_texture() const { return tex; }

private:
    gpu::graphics::texture tex;
};

inline void texture_2d::set_wrap_mode(wrap_mode wrap_mode)
{
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, wrap_mode);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, wrap_mode);
    GL_CHECK();
}

inline void texture_2d::set_max_anisotropy(GLfloat max_anisotropy)
{
    if (max_anisotropy < 1) {
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);
    }
    glTextureParameterf(tex, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);
    GL_CHECK();
}

} // namespace rendering

#endif // !__TEXTURE_2D_H_GUARD
