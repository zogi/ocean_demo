#ifndef __FRAMEBUFFER_H_GUARD
#define __FRAMEBUFFER_H_GUARD

#include <api/gpu/graphics.h>

namespace rendering {

class multisample_framebuffer {
public:
    multisample_framebuffer(int width, int height, int num_samples);
    ~multisample_framebuffer();
    multisample_framebuffer(const multisample_framebuffer&) = delete;
    multisample_framebuffer& operator=(const multisample_framebuffer&) = delete;

    void resize(int new_width, int new_height);
    void resolve_to_backbuffer();

private:
    int width, height, num_samples;

    gpu::graphics::framebuffer fbo;
    gpu::graphics::renderbuffer color_rb, depth_rb;
};

} // namespace renderenig

#endif // !__FRAMEBUFFER_H_GUARD
