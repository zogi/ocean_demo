#ifndef __SURFACE_H_GUARD
#define __SURFACE_H_GUARD

#include <SDL_surface.h>
#include <util/rect.h>

namespace os {

class surface {
public:
    surface(SDL_Surface *api_surface) : api_surface(api_surface) {}
    ~surface() { SDL_FreeSurface(api_surface); }
    surface(const surface &) = delete;
    surface(surface &&rhs)
    {
        api_surface = rhs.api_surface;
        rhs.api_surface = nullptr;
    }
    surface &operator=(const surface &) = delete;
    surface &operator=(surface &&rhs)
    {
        api_surface = rhs.api_surface;
        rhs.api_surface = nullptr;
        return *this;
    }

    static surface create_8bit_paletted(const util::extent &extent)
    {
        return SDL_CreateRGBSurface(0, extent.width, extent.height, 8, 0, 0, 0, 0);
    }
    static surface create_32bit_rgba(const util::extent &extent)
    {
        return SDL_CreateRGBSurface(0, extent.width, extent.height, 32, 0, 0, 0, 0xff);
    }

    util::extent get_extent() const { return { api_surface->w, api_surface->h }; }
    int get_pitch() const { return api_surface->pitch; };
    void *get_pixels() const { return api_surface->pixels; }

    void blit_to(surface &dst_surface, const util::offset &dst_offset)
    {
        SDL_Rect api_dst_rect;
        api_dst_rect.x = dst_offset.x;
        api_dst_rect.y = dst_offset.y;
        SDL_BlitSurface(api_surface, nullptr, dst_surface.api_surface, &api_dst_rect);
    }

private:
    SDL_Surface *api_surface;
};

} // namespace os

#endif // !__SURFACE_H_GUARD
