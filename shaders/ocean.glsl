// Ocean shader effect using screen-space projected grid.

// Draw command should use vertices such that they define a triangulated quad.
// Primitive mode should be TRIANGE_FAN, front face should be CCW.
// Instanced drawing should be used, where the number of instances should match the number of grid tiles:
//   number_of_instances = number_of_grid_tiles = grid_dim.x * grid_dim.y
// where grid_dim contains the grid dimensions (number of tiles along x any y direction).

// The original attributes of the input vertices are discarded.
// The model space vertex positions are calculated by the vertex shader.
// In model space the ocean plane coincides with the xz plane.

// Displacement mapping is used to render the ocean surface.
// The displacement is encoded in the RGB channels of the displacement texture map as a fraction of max_displacement, so
//   displacement = displacement_tex_sample * max_displacement
uniform sampler2D displacement_tex;
uniform vec3 max_displacement = vec3(5, 5, 5);
uniform vec3 units_per_meter;    // rendering units per displacement_map units (i.e. meters)
uniform vec3 tile_size_logical;  // in rendering units

vec3 get_displacement(vec2 p, vec2 dp_dx, vec2 dp_dy)
{
    return units_per_meter * max_displacement * (2.0f * textureGrad(displacement_tex, p, dp_dx, dp_dy).xyz - 1.0f);
}

// Displacement mapping is only used up to a certain distance. Beyond that a noise-perturbed normal is used.
// There is smooth transition between the two. The two thresholds of the transition range are define below.
#define DISPLACEMENT_MAPPING_DISTANCE_MIN 800.0f
#define DISPLACEMENT_MAPPING_DISTANCE_MAX 900.0f

// Camera data
struct camera_internal {
    ivec2 viewport_size;
    vec2 view_size;
    float z_far;
};

struct isometry {
    mat3 orientation;
    vec3 position;
};

uniform struct {
    camera_internal internal;
    isometry model_transform;  // Camera placement in model space.
} camera;

// The effect switches off below this altitude (in model space, so relative to water level).
#define MIN_CAM_HEIGHT 1e-2f

// The vertices of the projected screen-space grid are either regular or irregular.
// Regular vertices are those of which projections lies on the plane in front of the camera.
// Not regular vertices are irregular.
// Irregular grid vertices are snapped to the horizon.
// Triangles of all irregular vertices should be discarded.
#define VERTEX_STATE_REGULAR   0
#define VERTEX_STATE_IRREGULAR 1


//-------------------
#ifdef VERTEX_SHADER
//-------------------

layout(location = 0) in vec3 pos_in;

out vec3 model_pos_vs;
out vec3 displacement_vs;
out vec2 uv_vs, duv_dx_vs, duv_dy_vs;
flat out int vertex_state_vs;

// Screen-space projected grid
uniform ivec2 grid_dim;

// Vertex transform matrix
uniform mat4 proj_view_world_transform;

ivec2 index2_from_index_row_major(int index, ivec2 dim)
{
    return ivec2(index % dim.x, index / dim.x);
}

vec2 eye_grid_pos(ivec2 grid_point_index2)
{
    vec2 normalized_grid_pos = vec2(grid_point_index2) / vec2(grid_dim);
    return (normalized_grid_pos - 0.5f) * camera.internal.view_size;
}

void main()
{
    // Don't draw anything if camera is below water level. This is an above-water effect.
    if (camera.model_transform.position.y < MIN_CAM_HEIGHT) {
        vertex_state_vs = VERTEX_STATE_IRREGULAR;
        return;
    }

    vertex_state_vs = VERTEX_STATE_REGULAR;

    // Calculate grid vertex position in view space using the vertex ID and the instance ID.
    ivec2 i_vertex = index2_from_index_row_major(gl_VertexID, ivec2(2, 2));
    ivec2 i_instance = index2_from_index_row_major(gl_InstanceID, grid_dim);
    vec3 view_ray = vec3(eye_grid_pos(i_instance + i_vertex), -1.0f);

    vec3 model_ray = camera.model_transform.orientation * view_ray;
    model_ray = normalize(model_ray);

    // If the ray points upwards, parallel to the xz plane (misses the plane), or would point too far on the plane,
    // the grid vertex is considered irregular.
    float ray_y_threshold = min(-1e-4f, -camera.model_transform.position.y / camera.internal.z_far);
    if (model_ray.y >= ray_y_threshold) {
        vertex_state_vs = VERTEX_STATE_IRREGULAR;

        // Snap irregular vertices to the horizon (at z_far units away).
        // This way triangles intersecting the horizon curve are modified to follow it.
        model_ray.y = ray_y_threshold;
        model_ray = normalize(model_ray);
    }

    // Calculate vertex position in model space.
    float ray_distance = camera.model_transform.position.y / -model_ray.y;
    vec3 model_pos = camera.model_transform.position + ray_distance * model_ray;
    model_pos.y = 0.0f;  // Force model position to the xz plane.

    // Calculate eye-space derivatives of model_pos.
    vec3 cam_axis_x = camera.model_transform.orientation[0];
    vec3 cam_axis_y = camera.model_transform.orientation[1];
    vec2 d_ray_distance = ray_distance / -model_ray.y * vec2(cam_axis_x[1], cam_axis_y[1]);
    vec3 dmp_ds = d_ray_distance.s * model_ray + ray_distance * cam_axis_x;
    vec3 dmp_dt = d_ray_distance.t * model_ray + ray_distance * cam_axis_y;

    // Convert to screen-space derivative.
    vec2 viewport_size = camera.internal.viewport_size;
    vec2 units_per_pixel = camera.internal.view_size / viewport_size;
    vec3 dmp_dx = units_per_pixel.x * dmp_ds;
    vec3 dmp_dy = units_per_pixel.y * dmp_dt;

    // Calculate displacement.
    uv_vs = model_pos.xz / tile_size_logical.xz;
    duv_dx_vs = dmp_dx.xz / tile_size_logical.xz;
    duv_dy_vs = dmp_dy.xz / tile_size_logical.xz;
    displacement_vs = get_displacement(uv_vs, duv_dx_vs, duv_dy_vs);

    // Fade out displacement at the distance.
    float distance_to_camera = length(camera.model_transform.position - model_pos);
    displacement_vs *= 1 - smoothstep(DISPLACEMENT_MAPPING_DISTANCE_MIN, DISPLACEMENT_MAPPING_DISTANCE_MAX, distance_to_camera);

    model_pos += displacement_vs;
    model_pos_vs = model_pos;

    gl_Position = proj_view_world_transform * vec4(model_pos, 1);
}

#endif // VERTEX_SHADER


//---------------------
#ifdef GEOMETRY_SHADER
//---------------------

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 model_pos_vs[];
in vec3 displacement_vs[];
in vec2 uv_vs[], duv_dx_vs[], duv_dy_vs[];
flat in int vertex_state_vs[];

out vec3 model_pos_gs;
out vec2 uv_gs, duv_dx_gs, duv_dy_gs;
out vec3 displacement_gs;

void main()
{
    bool all_irregular = vertex_state_vs[0] == VERTEX_STATE_IRREGULAR &&
                         vertex_state_vs[1] == VERTEX_STATE_IRREGULAR &&
                         vertex_state_vs[2] == VERTEX_STATE_IRREGULAR;
    if (all_irregular)
        return;

    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;
        model_pos_gs = model_pos_vs[i];
        displacement_gs = displacement_vs[i];
        uv_gs = uv_vs[i];
        duv_dx_gs = duv_dx_vs[i];
        duv_dy_gs = duv_dy_vs[i];
        EmitVertex();
    }
}

#endif // GEOMETRY_SHADER


//---------------------
#ifdef FRAGMENT_SHADER
//---------------------

smooth in vec3 model_pos_gs;
smooth in vec3 displacement_gs;
smooth in vec2 uv_gs, duv_dx_gs, duv_dy_gs;

out vec3 color_out;

uniform sampler2D d_height_tex; // partial derivatives of the heightfield
uniform samplerCube sky_env;
uniform vec3 rf0_water = vec3(0.02f, 0.02f, 0.02f); // Real-Time Rendering 3rd ed. pg. 236
uniform vec3 diffuse_water = vec3(0.04f, 0.16f, 0.47f);

#define PI 3.14159265f
#define DERIV_EPS 1e-1f

#define saturate(what) clamp(what, 0.0f, 1.0f)
vec3 fresnel_reflectance(vec3 rf0, vec3 normal, vec3 incident)
{
    float ccos_theta_i = saturate(dot(normal, incident));
    // Schlick approximation
    return rf0 + (1.0f - rf0) * pow(1.0f - ccos_theta_i, 5.0f);
}

void main()
{
    // Get height slopes.
    vec2 d_height = textureGrad(d_height_tex, uv_gs, duv_dx_gs, duv_dy_gs).xy;

    // Approximate the derivative of the xz displacement vector field using first-order finite differences.
    vec2 hdisp_00 = displacement_gs.xz;
    vec2 hdisp_10 = get_displacement(uv_gs + vec2(DERIV_EPS, 0.0f), duv_dx_gs, duv_dy_gs).xz;
    vec2 hdisp_01 = get_displacement(uv_gs + vec2(0.0f, DERIV_EPS), duv_dx_gs, duv_dy_gs).xz;
    vec2 du_hdisp = (hdisp_10 - hdisp_00) / (DERIV_EPS * tile_size_logical.xz);
    vec2 dv_hdisp = (hdisp_01 - hdisp_00) / (DERIV_EPS * tile_size_logical.xz);

    vec3 dx = vec3(1.0f, 0.0f, 0.0f) + vec3(du_hdisp.x, d_height.x, du_hdisp.y);
    vec3 dz = vec3(0.0f, 0.0f, 1.0f) + vec3(dv_hdisp.x, d_height.y, dv_hdisp.y);

    // Fade out normal displacement with distance.
    float distance_to_camera = length(camera.model_transform.position - model_pos_gs);
    float mix_factor = smoothstep(DISPLACEMENT_MAPPING_DISTANCE_MIN, DISPLACEMENT_MAPPING_DISTANCE_MAX, distance_to_camera);
    vec3 normal = mix(normalize(cross(dx, -dz)), vec3(0, 1, 0), mix_factor);
    normal = normalize(normal);

    // Compute light color.
    vec3 eye = normalize(camera.model_transform.position - model_pos_gs);
    vec3 reflected_eye = reflect(-eye, normal);
    reflected_eye.y = abs(reflected_eye.y); // Don't sample the sky from below water level.
    vec3 fres = fresnel_reflectance(rf0_water, normal, eye);
    vec3 sky = texture(sky_env, reflected_eye, 1.0f).xyz;
    vec3 sky_radiance = textureLod(sky_env, reflected_eye, textureQueryLevels(sky_env)).xyz;
    vec3 water = diffuse_water * sky_radiance / PI;
    color_out = sky * fres + water * (1 - fres);
}

#endif // FRAGMENT_SHADER
