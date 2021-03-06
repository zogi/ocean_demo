#ifndef __FONT_H_GUARD
#define __FONT_H_GUARD

#include <SDL_ttf.h>
#include <api/math.h>
#include <api/os/surface.h>
#include <util/rect.h>

namespace io {

class font {
public:
    typedef SDL_Color color;
    font(const char *filename, int ptsize);
    ~font();
    int get_lineskip() const { return TTF_FontLineSkip(api_font); }
    os::surface render_text(const char *text, const color &color);

private:
    TTF_Font *api_font;
};

} // namespace io

#endif // !__FONT_H_GUARD
