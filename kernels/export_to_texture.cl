#define MAX_DISPLACEMENT_X 5.0f
#define MAX_DISPLACEMENT_Y 5.0f
#define MAX_DISPLACEMENT_Z 5.0f

float map_to_unorm(float x, float max_mag) {
    return (x + max_mag) / (2.0f * max_mag);
}

float4 pack_displacement(float x, float y, float z) {
    float4 res;
    res.x = map_to_unorm(x, MAX_DISPLACEMENT_X);
    res.y = map_to_unorm(y, MAX_DISPLACEMENT_Y);
    res.z = map_to_unorm(z, MAX_DISPLACEMENT_Z);
    res.w = 0;
    return res;
}

kernel void export_to_texture(global float *in, int N, int M, write_only image2d_t displacement_img, write_only image2d_t d_height_img) {
    int i = get_global_id(0), j = get_global_id(1);
    if (i > N || j > M) return;
    int row_stride = N + 2;
    int buf_stride = (N + 2) * M;
    float displacement_y = in[j * row_stride + i];
    float displacement_x = in[3 * buf_stride + (j%M) * row_stride + i%N];
    float displacement_z = in[4 * buf_stride + (j%M) * row_stride + i%N];
    float deriv_x = in[buf_stride + j * row_stride + i];
    float deriv_z = in[2 * buf_stride + j * row_stride + i];

    int2 dest_coords = (int2)(i, j);
    write_imagef(displacement_img, dest_coords, pack_displacement(displacement_x, displacement_y, displacement_z));
    write_imagef(d_height_img, dest_coords, (float4)(deriv_x, deriv_z, 0.0f, 0.0f));
}
