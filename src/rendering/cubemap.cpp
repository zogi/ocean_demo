#include <rendering/cubemap.h>

#include <cassert>
#include <vector>

#include <util/error.h>

namespace rendering {

cubemap::~cubemap()
{
    glDeleteTextures(1, &cubemap_texture);
}

void cubemap::load_from_file(const char *image_file)
{
    // Load image.
    io::image image(image_file);

    // Get image size and check if aspect ratio conforms to cubemap (4:3).
    auto width = ilGetInteger(IL_IMAGE_WIDTH);
    auto height = ilGetInteger(IL_IMAGE_HEIGHT);
    assert(width / 4 == height / 3);
    ILuint size = width / 4;
    assert(size <= GL_MAX_CUBE_MAP_TEXTURE_SIZE);

    // Allocate space for RGBA pixel data.
    const size_t face_size = size * size * 4;
    const size_t n_faces = 6;
    std::vector<unsigned char> pixel_data(face_size);

    // Create GL texture.
    glGenTextures(1, &cubemap_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

    // Copy image pixels to texture.
    // glspec45.core Table 9.2: Layer numbers for cube map texture faces.
    // 0:+X, 1:-X, 2:+Y, 3:-Y, 4:+Z, 5:-Z
    static const ILuint xofs[] = {2, 0, 1, 1, 1, 3};
    static const ILuint yofs[] = {1, 1, 0, 2, 1, 1};
    static const GLenum cube_face_target[] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                              GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                              GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
    for (size_t face = 0; face < n_faces; ++face) {
        image.copy_pixels(xofs[face] * size, yofs[face] * size, size, size, pixel_data.data());
        glTexImage2D(cube_face_target[face], 0, GL_RGBA8, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data.data());
        GL_CHECK();
    }

    // Generate mipmap and enable mipmap filtering.
    glGenerateTextureMipmap(cubemap_texture);
    GL_CHECK();
    glTextureParameteri(cubemap_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(cubemap_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void cubemap::bind(gpu::graphics::texture_unit unit)
{
    glBindTextureUnit(unit, cubemap_texture);
}

} // namespace rendering
