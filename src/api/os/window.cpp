#include <api/os/window.h>

#include "imgui_impl_sdl_gl3.h"

#include <api/gpu/graphics.h>
#include <util/error.h>

#define NOMINMAX
#include <SDL_syswm.h>

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

    ~window_api() { SDL_Quit(); }
};

} // namespace detail

window::window(const char *title, const util::extent &window_size, Uint32 sdl_window_flags)
    : window_handle(nullptr), gl_context(nullptr)
{
    static detail::window_api window_api;

    window_handle = SDL_CreateWindow(
        title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_size.width,
        window_size.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | sdl_window_flags);
    SDL_CHECK(window_handle != nullptr);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    gl_context = SDL_GL_CreateContext(window_handle);
    SDL_CHECK(gl_context != nullptr);

    gpu::graphics::init();

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGui_ImplSdlGL3_Init(window_handle);
    ImGui::StyleColorsDark();

    SDL_GL_SetSwapInterval(1);
}

window::~window()
{
    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();

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

} // unnamed namespace

window::display_handle window::get_display_handle() const
{
    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(window_handle, &wm_info);

#ifdef SDL_VIDEO_DRIVER_WINDOWS
    assert(wm_info.subsystem == SDL_SYSWM_WINDOWS);
    return reinterpret_cast<window::display_handle>(wm_info.info.win.hdc);
#elif defined SDL_VIDEO_DRIVER_X11
    assert(wm_info.subsystem == SDL_SYSWM_X11);
    return reinterpret_cast<window::display_handle>(wm_info.info.x11.display);
#endif

    DIE("Unsupported windowing system: %s\n", wm_name(wm_info.subsystem));
}

window::wm_type window::get_wm_type() const
{
    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(window_handle, &wm_info);

    switch (wm_info.subsystem) {
    case SDL_SYSWM_WINDOWS:
        return WM_TYPE_WINDOWS;
    case SDL_SYSWM_X11:
        return WM_TYPE_X11;
    default:
        DIE("Unsupported windowing system: %s\n", wm_name(wm_info.subsystem));
    }

    return WM_TYPE_UNKNOWN;
}

bool window::poll_event(os::event &event)
{
    SDL_Event *sdl_event = &event.get_api_event();
    while (SDL_PollEvent(sdl_event) != 0) {

        ImGui_ImplSdlGL3_ProcessEvent(sdl_event);

        // Check if it should be passed to the caller.
        const ImGuiIO &io = ImGui::GetIO();

        const bool is_keyboard_event = sdl_event->type == SDL_KEYDOWN ||
                                       sdl_event->type == SDL_KEYUP ||
                                       sdl_event->type == SDL_TEXTINPUT;
        if (io.WantCaptureKeyboard && is_keyboard_event)
            continue;

        const bool is_mouse_event =
            sdl_event->type == SDL_MOUSEMOTION || sdl_event->type == SDL_MOUSEBUTTONDOWN ||
            sdl_event->type == SDL_MOUSEBUTTONUP || sdl_event->type == SDL_MOUSEWHEEL;
        if (io.WantCaptureMouse && is_mouse_event)
            continue;

        // The event can be passed to the caller.
        return true;
    }
    return false;
}

void window::begin_frame() { ImGui_ImplSdlGL3_NewFrame(window_handle); }

void window::end_frame()
{
    ImGui::Render();
    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window_handle);
}

} // namespace os
