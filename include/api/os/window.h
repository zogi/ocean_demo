#ifndef __WINDOW_H_GUARD
#define __WINDOW_H_GUARD

#include <SDL.h>
#include <SDL_video.h>
#include <api/os/event.h>
#include <util/rect.h>

namespace os {

class window {
public:
    typedef util::extent size;
    typedef SDL_GLContext graphics_context;

    window(const char *title, const size& window_size, Uint32 sdl_window_flags = 0);
    ~window();
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    void swap_frame();
    inline window::size get_size() const;
    window::graphics_context get_graphics_context() const { return gl_context; }

    bool poll_event(os::event& event) { return SDL_PollEvent(&event.get_api_event()) != 0; }

private:
    SDL_Window *window_handle;
    SDL_GLContext gl_context;
};

inline void window::swap_frame()
{
    SDL_GL_SwapWindow(window_handle);
}

inline window::size window::get_size() const
{
    int width, height;
    SDL_GetWindowSize(window_handle, &width, &height);
    return { width, height };
}

} // namespace os

#endif // !__WINDOW_H_GUARD
