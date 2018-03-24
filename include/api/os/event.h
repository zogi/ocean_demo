#ifndef __EVENT_H_GUARD
#define __EVENT_H_GUARD

#include <SDL_events.h>
#include <api/math.h>
#include <util/rect.h>
#include <utility>

namespace os {

namespace api {

typedef SDL_Event event;
typedef decltype(std::declval<SDL_KeyboardEvent>().keysym.sym) keysym;
typedef decltype(std::declval<SDL_MouseButtonEvent>().button) mouse_button;
typedef decltype(std::declval<SDL_MouseMotionEvent>().state) mouse_button_state;

} // namespace api

class keyboard_event {
public:
    typedef api::keysym keysym;

    keyboard_event(const api::event &api_event) : api_event(api_event) {}

    bool is_key_down() const { return api_event.type == SDL_KEYDOWN; }
    bool is_key_up() const { return api_event.type == SDL_KEYUP; }
    keysym get_keysym() const { return api_event.key.keysym.sym; }
    const char *get_key_name() const { return SDL_GetKeyName(api_event.key.keysym.sym); }

private:
    const api::event &api_event;
};

class mouse_button_event {
public:
    enum mouse_button : api::mouse_button {
        MOUSE_BUTTON_LEFT = SDL_BUTTON_LEFT,
        MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
        MOUSE_BUTTON_RIGHT = SDL_BUTTON_RIGHT
    };
    typedef math::ivec2 mouse_pos;

    mouse_button_event(const api::event &api_event) : api_event(api_event) {}

    bool is_mouse_button_down() const { return api_event.type == SDL_MOUSEBUTTONDOWN; }
    bool is_mouse_button_up() const { return api_event.type == SDL_MOUSEBUTTONUP; }
    mouse_button get_mouse_button() const
    {
        return static_cast<mouse_button>(api_event.button.button);
    }
    mouse_pos get_mouse_button_pos() const { return { api_event.button.x, api_event.button.y }; }

private:
    const api::event &api_event;
};

class mouse_move_event {
public:
    enum mouse_button_state : api::mouse_button_state {
        MOUSE_BUTTON_STATE_LEFT = SDL_BUTTON_LMASK,
        MOUSE_BUTTON_STATE_MIDDLE = SDL_BUTTON_MMASK,
        MOUSE_BUTTON_STATE_RIGHT = SDL_BUTTON_RMASK
    };
    typedef math::ivec2 mouse_pos;

    mouse_move_event(const api::event &api_event) : api_event(api_event) {}

    mouse_button_state get_mouse_move_button_state() const
    {
        return static_cast<mouse_button_state>(api_event.motion.state);
    }
    mouse_pos get_mouse_move_pos() const { return { api_event.motion.x, api_event.motion.y }; }

private:
    const api::event &api_event;
};

class window;

class event {
public:
    event(api::event api_event = api::event()) : api_event(api_event) {}

    bool is_keyboard_event() const
    {
        return api_event.type == SDL_KEYDOWN || api_event.type == SDL_KEYUP;
    }
    bool is_mouse_move_event() const { return api_event.type == SDL_MOUSEMOTION; }
    bool is_mouse_button_event() const
    {
        return api_event.type == SDL_MOUSEBUTTONDOWN || api_event.type == SDL_MOUSEBUTTONUP;
    }
    bool is_window_resize_event() const
    {
        return api_event.type == SDL_WINDOWEVENT && api_event.window.event == SDL_WINDOWEVENT_RESIZED;
    }
    bool is_quit_event() const
    {
        return api_event.type == SDL_QUIT || api_event.type == SDL_APP_TERMINATING;
    }

    keyboard_event get_keyboard_event() const { return keyboard_event(api_event); }
    mouse_button_event get_mouse_button_event() const { return mouse_button_event(api_event); }
    mouse_move_event get_mouse_move_event() const { return mouse_move_event(api_event); }
    util::extent get_window_size() const
    {
        return { api_event.window.data1, api_event.window.data2 };
    }

private:
    friend class window;
    api::event get_api_event() const { return api_event; }
    api::event &get_api_event() { return api_event; }

    api::event api_event;
};

} // namespace os

#endif // !__EVENT_H_GUARD
