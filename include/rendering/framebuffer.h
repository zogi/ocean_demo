#ifndef __FRAMEBUFFER_H_GUARD
#define __FRAMEBUFFER_H_GUARD

#include <api/gpu/graphics.h>

namespace rendering {

class multisample_framebuffer {
public:
    multisample_framebuffer() : width(0), height(0), num_samples(0), fbo(0), color_rb(0), depth_rb(0) {}
    ~multisample_framebuffer();
    multisample_framebuffer(const multisample_framebuffer&) = delete;
    multisample_framebuffer& operator=(const multisample_framebuffer&) = delete;

    void allocate(int width, int height, int num_samples);
    void resize(int width, int height);
    void resolve_to_backbuffer();

private:
    void release_resources();

    int width, height, num_samples;

    gpu::graphics::framebuffer fbo;
    gpu::graphics::renderbuffer color_rb, depth_rb;
};

} // namespace renderenig

#endif // !__FRAMEBUFFER_H_GUARD
