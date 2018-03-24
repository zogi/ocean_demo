#ifndef __CUBEMAP_H_GUARD
#define __CUBEMAP_H_GUARD

#include <api/gpu/graphics.h>
#include <api/io/image.h>

namespace rendering {

class cubemap {
public:
    cubemap() : cubemap_texture(0) {}
    ~cubemap();
    cubemap(const cubemap &) = delete;
    cubemap &operator=(const cubemap &) = delete;

    void load_from_file(const char *image_file);
    void bind(gpu::graphics::texture_unit unit);

    typedef io::image::load_error load_error;

private:
    gpu::graphics::texture cubemap_texture;
};

} // namespace rendering

#endif // !__CUBEMAP_H_GUARD
