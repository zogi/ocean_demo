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

window::window(const char *title, const util::extent& window_size, Uint32 sdl_window_flags)
  : window_handle(nullptr), gl_context(nullptr)
{
    static detail::window_api window_api;

    window_handle = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                     window_size.width, window_size.height,
                                     SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | sdl_window_flags);
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

namespace {
    const char *wm_name(SDL_SYSWM_TYPE type)
    {
        switch (type) {
        case SDL_SYSWM_WINDOWS:
            return "Microsoft Windows";
        case SDL_SYSWM_X11:
            return "X Window System";
        case SDL_SYSWM_DIRECTFB:
            return "DirectFB";
        case SDL_SYSWM_COCOA:
            return "Apple Mac OS X";
        case SDL_SYSWM_UIKIT:
            return "Apple iOS";
        case SDL_SYSWM_WAYLAND:
            return "Wayland";
        case SDL_SYSWM_MIR:
            return "Mir";
        case SDL_SYSWM_WINRT:
            return "WinRT";
        case SDL_SYSWM_ANDROID:
            return "Android";
        case SDL_SYSWM_UNKNOWN:
        default:
            return "unknown";
        }
    }

    // Hack to extract winsys display handle
    union subsys_info {
        struct {
            intptr_t unused;
            intptr_t display;
        } win;

        struct {
            intptr_t display;
            // ...
        } x11;

        struct {
            intptr_t display;
            // ...
        } wayland;
    };

} // unnamed namespace

window::display_handle window::get_display_handle() const
{
    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(window_handle, &wm_info);
    subsys_info *subsys_info = reinterpret_cast<union subsys_info*>(&wm_info.info);

    switch (wm_info.subsystem) {
    case SDL_SYSWM_WINDOWS:
        return subsys_info->win.display;
        break;
    case SDL_SYSWM_X11:
        return subsys_info->x11.display;
        break;
    case SDL_SYSWM_WAYLAND:
        return subsys_info->wayland.display;
        break;
    default:
        DIE("Unsupported windowing system: %s\n", wm_name(wm_info.subsystem));
    }
    return window::display_handle();
}

window::wm_type window::get_wm_type() const
{
    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(window_handle, &wm_info);
    return wm_info.subsystem;
}

} // namespace os
