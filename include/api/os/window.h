#ifndef __WINDOW_H_GUARD
#define __WINDOW_H_GUARD

#include <SDL.h>
#include <SDL_video.h>
#include <api/os/event.h>

namespace os {

class window {
public:
    typedef os::window_size size;
    typedef SDL_GLContext graphics_context;

    window(int width, int height, const char *title);
    ~window();
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    void swap_frame();
    inline window::size get_size() const;
    window::graphics_context get_graphics_context() const { return gl_context; }

    // event_handler has to have a public method named 'handle_event' which accepts a single os::event argument.
    template <typename event_handler>
    void process_events(event_handler event_handler_object = event_handler());

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

template<typename event_handler>
inline void window::process_events(event_handler event_handler_object)
{
    api::event api_event;
    while (SDL_PollEvent(&api_event)) {
        event_handler_object.handle_event(os::event(api_event));
    }
}

} // namespace os

#endif // !__WINDOW_H_GUARD
