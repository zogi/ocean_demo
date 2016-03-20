#ifndef __WINDOW_H_GUARD
#define __WINDOW_H_GUARD

#include <SDL.h>
#include <SDL_video.h>

#include <api/os/event.h>
#include <util/rect.h>

namespace os {

class window {
public:
    typedef SDL_GLContext graphics_context;
    typedef intptr_t display_handle;
    enum wm_type { WM_TYPE_UNKNOWN = 0, WM_TYPE_WINDOWS = 1, WM_TYPE_X11 = 2 };

    window(const char *title, const util::extent& window_size, Uint32 sdl_window_flags = 0);
    ~window();
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    inline util::extent get_extent() const;
    graphics_context get_graphics_context() const { return gl_context; }
    display_handle get_display_handle() const;
    wm_type get_wm_type() const;

    bool poll_event(os::event& event) { return SDL_PollEvent(&event.get_api_event()) != 0; }
    void swap_frame();

private:
    SDL_Window *window_handle;
    SDL_GLContext gl_context;
};

inline void window::swap_frame()
{
    SDL_GL_SwapWindow(window_handle);
}

inline util::extent window::get_extent() const
{
    int width, height;
    SDL_GetWindowSize(window_handle, &width, &height);
    return { width, height };
}

} // namespace os

#endif // !__WINDOW_H_GUARD
