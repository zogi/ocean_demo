#ifndef __MATH_H_GUARD
#define __MATH_H_GUARD

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace math {

typedef float real;

const real pi = glm::pi<real>();
const real half_pi = glm::half_pi<real>();
const real two_pi = glm::two_pi<real>();

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
typedef glm::tvec2<real> vec2;
typedef glm::tvec3<real> vec3;
typedef glm::tvec4<real> vec4;
typedef glm::tmat2x2<real> mat2;
typedef glm::tmat3x3<real> mat3;
typedef glm::tmat4x4<real> mat4;

using glm::tan;
using glm::length;
using glm::normalize;
using glm::dot;
using glm::cross;
using glm::transpose;
using glm::lookAt;
using glm::perspective;
using glm::value_ptr;

class spherical_angles {
public:
    spherical_angles() {}
    spherical_angles(real azimuthal, real polar) { set_azimuthal(azimuthal); set_polar(polar); }
    spherical_angles(const vec2& angles) { set_azimuthal(angles.x); set_polar(angles.y); }
    spherical_angles(const vec3& cartesian)
    {
        real radius = math::length(cartesian);
        set_polar(glm::acos(cartesian.y / radius));
        set_azimuthal(glm::atan(cartesian.z, cartesian.x));
    }

    vec3 cartesian() const
    {
        real sin_azimuthal = glm::sin(get_azimuthal());
        real cos_azimuthal = glm::cos(get_azimuthal());
        real sin_polar = glm::sin(get_polar());
        real cos_polar = glm::cos(get_polar());
        return vec3(cos_azimuthal * sin_polar, cos_polar, sin_azimuthal * sin_polar);
    }

    real get_azimuthal() const { return rep.x; }
    real get_polar() const { return rep.y; }
    void set_azimuthal(real azimuthal) { rep.x = azimuthal; }
    void set_polar(real polar) { rep.y = glm::clamp(polar, eps, pi - eps); }

private:
    static constexpr real eps = real(1e-5);
    vec2 rep;
};

} // namespace math

#endif // !__MATH_H_GUARD
