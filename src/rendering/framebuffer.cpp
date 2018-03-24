#include <rendering/framebuffer.h>
#include <util/error.h>

namespace rendering {

multisample_framebuffer::multisample_framebuffer(int width, int height, int num_samples)
    : width(width), height(height), num_samples(num_samples)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    gpu::graphics::renderbuffer rbs[2];
    glGenRenderbuffers(2, rbs);

    // Color.
    color_rb = rbs[0];
    glBindRenderbuffer(GL_RENDERBUFFER, color_rb);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, num_samples, GL_RGB8, width, height);
    GL_CHECK();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rb);
    GL_CHECK();

    // Depth.
    depth_rb = rbs[1];
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, num_samples, GL_DEPTH_COMPONENT, width, height);
    GL_CHECK();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
    GL_CHECK();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

multisample_framebuffer::~multisample_framebuffer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &color_rb);
    glDeleteRenderbuffers(1, &depth_rb);
}

void multisample_framebuffer::resize(int new_width, int new_height)
{
    multisample_framebuffer new_fb(new_width, new_height, num_samples);
    std::swap(width, new_fb.width);
    std::swap(height, new_fb.height);
    std::swap(fbo, new_fb.fbo);
    std::swap(color_rb, new_fb.color_rb);
    std::swap(depth_rb, new_fb.depth_rb);
}

void multisample_framebuffer::activate() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); }

void multisample_framebuffer::resolve_to_backbuffer()
{
    // Blit from framebuffer to backbuffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

} // namespace rendering
