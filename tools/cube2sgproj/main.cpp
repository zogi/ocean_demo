#include <api/os/window.h>
#include <rendering/cubemap.h>
#include <rendering/quad.h>
#include <rendering/shader_effect.h>
#include <iostream>
#include <vector>
#include <cstdio>

int main(int argc, char *argv[])
{
    // Check command line args.
    if (argc != 2 && argc != 3) {
        std::cerr << "usage: " << argv[0] << " image_file [output_res]" << std::endl;
        return 1;
    }

    // Parse x and y resolution of the output if given.
    int res = 1024;
    if (argc == 3) {
        res = atoi(argv[2]);
    }

    // Create hidden windod (this will initialize OpenGL).
    os::window w("", util::extent(res, res), SDL_WINDOW_HIDDEN);
    glDisable(GL_MULTISAMPLE);

    // Load cube map.
    rendering::cubemap cm;
    cm.load_from_file(argv[1]);

    // Output resolution

    // Create off-srceen framebuffer.
    GLuint fb = 0;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    GL_CHECK();

    GLuint rb_color = 0;
    glGenRenderbuffers(1, &rb_color);
    glBindRenderbuffer(GL_RENDERBUFFER, rb_color);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, res, res);
    GL_CHECK();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb_color);
    GL_CHECK();

    GLuint rb_depth = 0;
    glGenRenderbuffers(1, &rb_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rb_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, res, res);
    GL_CHECK();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb_depth);
    GL_CHECK();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "error: framebuffer is incomplete." << std::endl;
        return 1;
    }
    GL_CHECK();

    // Create filtering shader.
    rendering::shader_effect shader;
    shader.load_shaders("sgproj.glsl", rendering::shader_type::VERTEX | rendering::shader_type::FRAGMENT);
    shader.use();

    // Attach cube map to cube_tex sampler.
    cm.bind(1);
    shader.set_parameter("cube_tex", 1);

    // Add rim around circle for filtering.
    const int rim_pixels = 2;
    shader.set_parameter("rim", 2 * rim_pixels / float(res));

    // Draw quad.
    glClear(GL_DEPTH_BUFFER_BIT);
    rendering::quad q;
    q.draw();

    // Save framebbuffer to file.
    std::vector<uint8_t> pixels(res * res * 3 * sizeof(uint8_t));
    glReadPixels(0, 0, res, res, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    ILuint img;
    ilGenImages(1, &img);
    ilBindImage(img);
    ilTexImage(res, res, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, pixels.data());
    std::remove("output.png");
    ilSaveImage("output.png");

    return 0;
}
