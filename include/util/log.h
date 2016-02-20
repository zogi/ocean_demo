#ifndef __LOG_H_GUARD
#define __LOG_H_GUARD

#define LOG(...) ::util::log(__FILE__, __LINE__, __VA_ARGS__)

namespace util {

void log(const char *file, int line, const char *format, ...);

} // namespace util

#endif // !__LOG_H_GUARD
