#include <rendering/text_renderer.h>

#include <algorithm>
#include <sstream>
#include <vector>
#include <rendering/texture_2d.h>

namespace rendering {

text_renderer::text_renderer() : font("OpenSans-Regular.ttf", 26), color({0xff, 0xff, 0xff, 0xff})
{
    text_shader.load_shaders("shaders/text.glsl", shader_type::VERTEX | shader_type::FRAGMENT);
    text_shader.set_z_test_state(false);
    text_shader.set_z_write_state(false);
}

void text_renderer::render_text(const std::string& text, const util::offset& offset)
{
    constexpr gpu::graphics::texture_unit text_texture_unit = 4;

    std::stringstream ss(text);
    std::string line;

    // Count lines.
    int nlines = static_cast<int>(std::count(text.begin(), text.end(), '\n'));
    if (text.back() != '\n')
        ++nlines;

    // Render lines to separate surfaces.
    std::vector<os::surface> line_surfaces;
    line_surfaces.reserve(nlines);
    int text_width = 0;
    while (std::getline(ss, line, '\n')) {
        line_surfaces.push_back(font.render_text(line.c_str(), color));
        text_width = std::max(text_width, line_surfaces.back().get_extent().width);
    }

    // Blit line surfaces to a single text surface.
    os::surface text_surface = os::surface::create_32bit_rgba(util::extent(text_width, nlines * font.get_lineskip()));
    util::offset line_offset(0, 0);
    for (auto& line_surface : line_surfaces) {
        line_surface.blit_to(text_surface, line_offset);
        line_offset.y += font.get_lineskip();
    }

    // Create texture from text_surface.
    auto texture_extent = util::extent(text_surface.get_pitch() / 4, text_surface.get_extent().height);
    auto text_texture = texture_2d(texture_extent, texture_2d::TEXTURE_FORMAT_RGBA8, text_surface.get_pixels());
    text_texture.set_mag_filter(texture_2d::MAG_FILTER_LINEAR);
    text_texture.set_min_filter(texture_2d::MIN_FILTER_LINEAR);
    text_texture.set_wrap_mode(texture_2d::WRAP_MODE_CLAMP_TO_EDGE);
    text_texture.bind(text_texture_unit);

    // Render the texture.
    text_shader.use();
    text_shader.set_parameter("text", text_texture_unit);
    text_shader.set_parameter("offset", offset);
    text_shader.set_parameter("extent", text_surface.get_extent());
    static GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    util::extent viewport_size(viewport[2], viewport[3]);
    text_shader.set_parameter("viewport_size", viewport_size);

    glEnable(GL_BLEND);
    quad.draw();
    glDisable(GL_BLEND);
}

} // namespace rendering
