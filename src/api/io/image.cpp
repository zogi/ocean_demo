#include <api/io/image.h>

#include <IL/ilu.h>

#include <util/error.h>

namespace io {

namespace detail {

#define IL_CHECK() detail::il_check(__FILE__, __LINE__)
inline void il_check(const char *file, int line)
{
    auto il_error = ilGetError();
    if (il_error != IL_NO_ERROR) {
        util::die(file, line, "IL error %d: %s\n", il_error, iluErrorString(il_error));
    }
}

struct image_api {
    image_api()
    {
        ilInit();
        iluInit();
    }
};

} // namespace detail

image::image(const char *filename)
{
    static detail::image_api image_api;

    ilGenImages(1, &image_handle);
    ilBindImage(image_handle);
    ilLoadImage(filename);
    if (ilGetError() == IL_COULD_NOT_OPEN_FILE) {
        throw load_error();
    }
    IL_CHECK();
}

image::~image() { ilDeleteImages(1, &image_handle); }

void image::copy_pixels(int x_offset, int y_offset, int width, int height, void *destination) const
{
    ilBindImage(image_handle);
    ilCopyPixels(x_offset, y_offset, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, destination);
    IL_CHECK();
}

} // namespace io
