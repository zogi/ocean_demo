#ifndef __TEXT_RENDERER_H_GUARD
#define __TEXT_RENDERER_H_GUARD

#include <api/io/font.h>
#include <rendering/quad.h>
#include <rendering/shader_effect.h>
#include <util/rect.h>

namespace rendering {

class text_renderer {
public:
    text_renderer();
    void render_text(const char *text, const util::offset& offset);
    void set_text_color(const io::font::color& color) { this->color = color; }
private:
    io::font font;
    io::font::color color;
    rendering::quad quad;
    rendering::shader_effect text_shader;
};

} // namespace rendering

#endif // !__TEXT_RENDERER_H_GUARD
