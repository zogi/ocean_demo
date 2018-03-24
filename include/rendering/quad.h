#ifndef __QUAD_H_GUARD
#define __QUAD_H_GUARD

#include <api/gpu/graphics.h>

namespace rendering {

class quad {
public:
    quad();
    ~quad();
    quad(const quad &) = delete;
    quad &operator=(const quad &) = delete;

    void draw();
    void draw_patch();
    void draw_instanced(size_t num_instances);
    void draw_patch_instanced(size_t num_instances);

private:
    GLuint vao, vbo;
};

} // namespace rendering

#endif // __QUAD_H_GUARD
