#include <rendering/text_renderer.h>

#include <rendering/texture_2d.h>

namespace rendering {

text_renderer::text_renderer() : font("OpenSans-Regular.ttf", 26), color({0xff, 0xff, 0xff, 0xff})
{
    text_shader.load_shaders("shaders/text.glsl", shader_type::VERTEX | shader_type::FRAGMENT);
    text_shader.set_z_test_state(false);
    text_shader.set_z_write_state(false);
}

void text_renderer::render_text(const char *text, const util::offset& offset)
{
    constexpr gpu::graphics::texture_unit text_texture_unit = 4;

    auto text_surface = font.render_text(text, color);
    util::extent extent = text_surface.get_extent();
    extent.width = text_surface.get_pitch();

    auto text_texture = texture_2d(text_surface.get_extent(), texture_2d::TEXTURE_FORMAT_R8I, text_surface.get_pixels());
    text_texture.set_mag_filter(texture_2d::MAG_FILTER_NEAREST);
    text_texture.set_min_filter(texture_2d::MIN_FILTER_NEAREST);
    text_texture.set_wrap_mode(texture_2d::WRAP_MODE_CLAMP_TO_EDGE);
    text_texture.bind(text_texture_unit);

    text_shader.use();
    text_shader.set_parameter("text", text_texture_unit);
    text_shader.set_parameter("offset", offset);
    text_shader.set_parameter("extent", text_surface.get_extent());
    static GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    util::extent viewport_size(viewport[2], viewport[3]);
    text_shader.set_parameter("viewport_size", viewport_size);

    quad.draw();
}

} // namespace rendering
