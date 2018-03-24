#ifndef __IMAGE_H_GUARD
#define __IMAGE_H_GUARD

#include <IL/il.h>

namespace io {

class image {
public:
    image(const char *filename);
    ~image();
    void copy_pixels(int x_offset, int y_offset, int width, int height, void *destination) const;
    struct load_error {
    };

private:
    ILuint image_handle;
};

} // namespace io

#endif // !__IMAGE_H_GUARD
