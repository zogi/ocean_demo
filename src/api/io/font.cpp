#include <api/io/font.h>
#include <util/error.h>

namespace io {

namespace detail {

#define TTF_CHECK(what) ::io::detail::ttf_check((what), __FILE__, __LINE__)
void ttf_check(bool status, const char *file, int line)
{
    if (!status) {
        util::die(file, line, "%s(%d): TTF error: %s\n", TTF_GetError());
    }
}

class ttf_api {
public:
    ttf_api()
    {
        TTF_CHECK(TTF_Init() == 0);
    };

    ~ttf_api()
    {
        TTF_Quit();
    }
};

} // namespace detail


font::font(const char *filename, int ptsize)
{
    static detail::ttf_api ttf_api;

    api_font = TTF_OpenFont(filename, ptsize);
    TTF_CHECK(api_font != nullptr);
}

font::~font()
{
    TTF_CloseFont(api_font);
}

os::surface font::render_text(const char *text, const color& color)
{
    auto text_surface = TTF_RenderText_Solid(api_font, text, color);
    TTF_CHECK(text_surface != 0);
    return text_surface;
}

} // namespace io
