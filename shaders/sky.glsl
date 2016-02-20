// Sky box effect.

// Draw command should use vertices such that they define a triangulated quad.
// Primitive mode should be TRIANGE_FAN, front face should be CCW.

// The original position of the input vertices are discarded, a screen-filling
// quad is automatically created using the vertex ID.

//-------------------
#ifdef VERTEX_SHADER
//-------------------

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 uv_in;

out vec2 uv_vs;

ivec2 index2_from_index_row_major(int index, ivec2 dim)
{
    return ivec2(index % dim.x, index / dim.x);
}

void main()
{
    vec2 pos = index2_from_index_row_major(gl_VertexID, ivec2(2, 2));
    gl_Position = vec4(2 * pos - 1, 0, 1);
    uv_vs = pos;
}

#endif // VERTEX_SHADER


//---------------------
#ifdef FRAGMENT_SHADER
//---------------------

smooth in vec2 uv_vs;
out vec3 color_out;

uniform samplerCube sky_env;
uniform mat3 view_rotation;
uniform vec2 eye_size;

void main()
{
    vec3 ray = vec3(eye_size * (uv_vs - 0.5f), -1.0f);
    vec3 cube_ray = view_rotation * ray;
    color_out = texture(sky_env, cube_ray).xyz;
}

#endif // FRAGMENT_SHADER
