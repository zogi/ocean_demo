#ifndef __TIMING_H_GUARD
#define __TIMING_H_GUARD

#include <api/gpu/graphics.h>

namespace util {

class graphics_timer
{
public:
    graphics_timer() { glCreateQueries(GL_TIME_ELAPSED, 1, &api_timer); }
    ~graphics_timer() { glDeleteQueries(1, &api_timer); }
    graphics_timer(const graphics_timer&) = delete;
    graphics_timer& operator=(const graphics_timer&) = delete;

    void start() { glBeginQuery(GL_TIME_ELAPSED, api_timer); }
    double stop_and_get_milliseconds()
    {
        glEndQuery(GL_TIME_ELAPSED);
        GLint64 nanoseconds;
        glGetQueryObjecti64v(api_timer, GL_QUERY_RESULT, &nanoseconds);
        return nanoseconds * 1e-6;
    }

private:
    GLuint api_timer;
};

template <typename Timer>
class scoped_timer_t {
public:
    scoped_timer_t(Timer& timer, double& result) : timer(timer), result(result) { timer.start(); }
    ~scoped_timer_t() { result = timer.stop_and_get_milliseconds(); }

private:
    Timer& timer;
    double& result;
};

template <typename Timer>
scoped_timer_t<Timer> scoped_timer(Timer& timer, double& result) {
    return { timer, result };
}

} // namespace util

#endif // !__TIMING_H_GUARD
