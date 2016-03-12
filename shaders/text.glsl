#ifdef VERTEX_SHADER

uniform ivec2 offset;
uniform ivec2 extent;
uniform ivec2 viewport_size;

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 uv_in;

out vec2 uv_vs;

ivec2 index2_from_index_row_major(int index, ivec2 dim)
{
    return ivec2(index % dim.x, index / dim.x);
}

void main()
{
    uv_vs = index2_from_index_row_major(gl_VertexID, ivec2(2, 2));
    ivec2 offset_gl = ivec2(offset.x, viewport_size.y - offset.y - extent.y);
    vec2 pos = (vec2(offset_gl) + uv_vs * vec2(extent)) / vec2(viewport_size);
    gl_Position = vec4(2 * pos - 1, 0, 1);
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

uniform sampler2D text;

in vec2 uv_vs;
out vec4 color_out;

void main()
{
    vec2 uv = vec2(uv_vs.x, 1 - uv_vs.y);
    color_out = texture2D(text, uv);
}

#endif // FRAGMENT_SHADER