#define __STDC_WANT_SECURE_LIB__ 0 // disable VS version of localtime_s
#define _CRT_SECURE_NO_WARNINGS 1 // suppress VS warning about using localtime
#define __STDC_WANT_LIB_EXT1__ 1 // needed for standard localtime_s
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <util/log.h>

#ifndef __STDC_LIB_EXT1__
#include <cstring>
namespace {
struct tm *localtime_s(const time_t *time, struct tm *result)
{
    struct tm *tm_data = localtime(time);
    memcpy(result, tm_data, sizeof(struct tm));
    return tm_data;
}
} // namespace
#endif

namespace util {

void log(const char *file, int line, const char *format, ...)
{
    va_list(args);

    auto now = std::chrono::system_clock::now();

    // HH:MM:SS
    auto now_t = std::chrono::system_clock::to_time_t(now);
    struct tm now_cal;
    localtime_s(&now_t, &now_cal);
    static char timestr[20];
    strftime(timestr, sizeof timestr, "%H:%M:%S", &now_cal);

    // milliseconds
    auto now_beg_sec = std::chrono::system_clock::from_time_t(std::mktime(&now_cal));
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - now_beg_sec);

    printf("[%s.%03lld] %s line %d: ", timestr, millis.count(), file, line);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

} // namespace util
