#ifndef __MAIN_WINDOW_H_GUARD
#define __MAIN_WINDOW_H_GUARD

#include <api/os/window.h>
#include <rendering/framebuffer.h>

class main_window {
public:
    enum run_state { RUN_STATE_UNINITIALIZED, RUN_STATE_RUNNING, RUN_STATE_QUITTING };

    main_window(int width, int height, const char *title, int num_framebuffer_samples);
    main_window(const main_window&) = delete;
    main_window& operator=(const main_window&) = delete;

    void swap_frame();
    auto get_size() const { return window.get_size(); }
    run_state get_run_state() const { return run_state; }
    auto get_graphics_context() const { return window.get_graphics_context(); }

    // Enables iteration over unprocessed events.
    class event_sequence {
    public:
        event_sequence(main_window& parent) : parent(parent) {}
        typedef os::event elem_type;
        class iterator {
        public:
            typedef event_sequence::elem_type value_type;
            iterator(main_window *parent = nullptr) : parent(parent) {}
            bool operator==(const iterator& rhs) { return parent == nullptr && rhs.parent == nullptr; }
            bool operator!=(const iterator& rhs) { return !(*this == rhs); }
            iterator& operator++() { if (!parent->poll_event(value)) parent = nullptr; return *this; }
            value_type& operator*() { return value; }
        private:
            main_window *parent;
            value_type value;
        };
        iterator begin() { return iterator(&parent); }
        iterator end() { return iterator(); }
    private:
        main_window& parent;
    };
    event_sequence unprocessed_events() { return event_sequence(*this); }

private:
    friend class event_sequence::iterator;
    bool poll_event(os::event& event);

    void handle_quit_event();
    void handle_resize_event(const os::window::size& size);
    void handle_keyboard_event(const os::keyboard_event& event);

    os::window window;
    rendering::multisample_framebuffer framebuffer;
    run_state run_state;
};

#endif // !__MAIN_WINDOW_H_GUARD
