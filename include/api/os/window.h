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

    window(const size& window_size, const char *title);
    ~window();
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    void swap_frame();
    inline window::size get_size() const;
    window::graphics_context get_graphics_context() const { return gl_context; }

    // Enables iteration over unprocessed events.
    class event_sequence {
    public:
        event_sequence(window& parent) : parent(parent) {}
        typedef os::event elem_type;
        class iterator {
        public:
            typedef event_sequence::elem_type value_type;
            iterator(window *parent = nullptr) : parent(parent) {}
            bool operator==(const iterator& rhs) { return parent == nullptr && rhs.parent == nullptr; }
            bool operator!=(const iterator& rhs) { return !(*this == rhs); }
            iterator& operator++() { if (!parent->poll_event(value)) parent = nullptr; return *this; }
            value_type& operator*() { return value; }
        private:
            window *parent;
            value_type value;
        };
        iterator begin() { return iterator(&parent); }
        iterator end() { return iterator(); }
    private:
        window& parent;
    };
    event_sequence unprocessed_events() { return event_sequence(*this); }

private:
    friend class event_sequence::iterator;
    bool poll_event(os::event& out_event) { return SDL_PollEvent(&out_event.get_api_event()) != 0; }

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
