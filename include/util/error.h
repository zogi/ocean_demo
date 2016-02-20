#ifndef __ERROR_H_GUARD
#define __ERROR_H_GUARD

#include <cstdlib>
#include <utility>
#include <util/log.h>

#define DIE(...) ::util::die(__FILE__, __LINE__, __VA_ARGS__)

namespace util {

template <typename ...Args>
void die(const char *file, int line, Args&&... args) {
    util::log(file, line, std::forward<Args>(args)...);
    abort();
}

} // namespace util

#endif // __ERROR_H_GUARD
