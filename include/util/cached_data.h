#ifndef __CACHED_DATA_H_GUARD
#define __CACHED_DATA_H_GUARD

namespace util {

template <typename T>
class cached_data {
public:
    cached_data(): dirty(true) {}
    cached_data(const T& data) : data(data), dirty(false) {}
    const T& get() const { return data; }
    void set(const T& new_data) { data = new_data; dirty = false; }
    bool is_dirty() const { return dirty; }
    void invalidate() { dirty = true; }
private:
    T data;
    bool dirty;
};

} // namespace util

#endif // !__CACHED_DATA_H_GUARD
