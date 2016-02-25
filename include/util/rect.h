#ifndef __RECT_H_GUARD
#define __RECT_H_GUARD

namespace util {

struct offset
{
    offset(int x, int y) : x(x), y(y) {}
    int x;
    int y;
};

struct extent
{
    extent(int width, int height) : width(width), height(height) {}
    int width;
    int height;
};

struct rect
{
    rect(const offset& offset, const extent& extent) : offset(offset), extent(extent) {}
    offset offset;
    extent extent;
};

} // namespace util

#endif // !__RECT_H_GUARD
