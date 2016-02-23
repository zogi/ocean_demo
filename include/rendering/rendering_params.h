#ifndef __RENDERING_PARAMS_H_GUARD
#define __RENDERING_PARAMS_H_GUARD

#include <api/math.h>

namespace rendering {

struct rendering_params {
    int multisampling_sample_count;
    float texture_max_anisotropy;
};

} // namespace rendering

#endif // !__RENDERING_PARAMS_H_GUARD
