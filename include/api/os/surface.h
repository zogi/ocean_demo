#ifndef __SURFACE_H_GUARD
#define __SURFACE_H_GUARD

#include <SDL_surface.h>
#include <util/rect.h>

namespace os {

class surface {
public:
    surface(SDL_Surface *api_surface) : api_surface(api_surface) {}
    ~surface() { SDL_FreeSurface(api_surface); }
    surface(const surface&) = delete;
    surface(surface&& rhs) { api_surface = rhs.api_surface; rhs.api_surface = nullptr; }
    surface& operator=(const surface&) = delete;
    surface& operator=(surface&& rhs) { api_surface = rhs.api_surface; rhs.api_surface = nullptr; return *this; }
    int get_pitch() const { return api_surface->pitch; };
    util::extent get_extent() const { return { api_surface->w, api_surface->h }; }
    void *get_pixels() const { return api_surface->pixels; }
private:
    SDL_Surface *api_surface;
};

} // namespace os

#endif // !__SURFACE_H_GUARD
