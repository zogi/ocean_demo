#include <api/os/window.h>

#include <util/error.h>
#include <api/gpu/graphics.h>

namespace os {

namespace detail {

#define SDL_CHECK(what) ::os::detail::sdl_check((what), __FILE__, __LINE__)
void sdl_check(bool status, const char *file, int line)
{
    if (!status) {
        util::die(file, line, "%s(%d): SDL error: %s\n", SDL_GetError());
    }
}

class window_api {
public:
    window_api()
    {
        SDL_CHECK(SDL_Init(SDL_INIT_VIDEO) == 0);

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    };

    ~window_api()
    {
        SDL_Quit();
    }
};

} // namespace detail

window::window(const size& window_size, const char *title)
    : window_handle(nullptr), gl_context(nullptr)
{
    static detail::window_api window_api;

    window_handle = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_size.get_width(), window_size.get_height(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_CHECK(window_handle != nullptr);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    gl_context = SDL_GL_CreateContext(window_handle);
    SDL_CHECK(gl_context != nullptr);

    gpu::graphics::init();

    SDL_GL_SetSwapInterval(1);
}

window::~window()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window_handle);
}

} // namespace os
