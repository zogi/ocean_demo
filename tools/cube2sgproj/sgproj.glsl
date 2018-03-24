#ifdef VERTEX_SHADER

uniform ivec2 viewport_size;

layout(location = 0) in vec3 pos_in;

out vec2 uv_vs;

ivec2 index2_from_index_row_major(int index, ivec2 dim)
{
    return ivec2(index % dim.x, index / dim.x);
}

void main()
{
    uv_vs = index2_from_index_row_major(gl_VertexID, ivec2(2, 2));
    gl_Position = vec4(2 * uv_vs - 1, 0, 1);
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

uniform samplerCube cube_tex;

in vec2 uv_vs;
out vec3 color_out;

uniform float rim;

void main()
{
	vec2 v = 2 * uv_vs.xy - 1;
	float normsq = dot(v, v);
	float norm = sqrt(normsq);
	if (normsq > 1) {
		v /= norm;
		normsq = 1;
	}
	vec3 dir;
	dir.x = 2 * v.x / (1 + normsq);
	dir.z = 2 * v.y / (1 + normsq);
	dir.y = sqrt(max(0, 1 - dir.x * dir.x - dir.z * dir.z));
	color_out = texture(cube_tex, dir).xyz;

	if (norm > 1 + rim)
		color_out = vec3(0, 0, 0);
}

#endif // FRAGMENT_SHADER
