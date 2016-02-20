typedef float real;

constant real two_pi = 6.28318530718;
constant real T = 10.0; // time period in seconds

real periodize_omega(real omega, real period)
{
    real omega_0 = two_pi / period;
    return floor(omega / omega_0) * omega_0;
}

real dispersion_relation(real k)
{
    return periodize_omega(sqrt(9.80665 * k), T);
}

kernel void phase_shift(global const real *in, real Lx, real Lz, int N, int M, real t, global real *out)
{
    int i = get_global_id(0), j = get_global_id(1);
    if (i > N / 2 || j > M) return;

    real k_x = two_pi * i / Lx;
    int jj = (j + M / 2) % M - M / 2;
    real k_z = two_pi * jj / Lz;
    real k = sqrt(k_x * k_x + k_z * k_z);

    real omega = dispersion_relation(k);
    real cos_omegat;
    real sin_omegat = sincos(omega * t, &cos_omegat);

    int in_stride = N + 2;
    int in_idx = in_stride * j + 2 * i;
    real h_re = in[in_idx+0];
    real h_im = in[in_idx+1];
    real out_re = h_re * cos_omegat - h_im * sin_omegat;
    real out_im = h_im * cos_omegat + h_re * sin_omegat;
    int out_row_stride = N + 2;
    int out_buf_stride = out_row_stride * M;
    int out_idx = out_row_stride * j + 2 * i;
    int out_deriv_x_base = out_buf_stride + out_idx;
    int out_deriv_z_base = 2*out_buf_stride + out_idx;
    int out_disp_x_base = 3*out_buf_stride + out_idx;
    int out_disp_z_base = 4*out_buf_stride + out_idx;
    out[out_idx+0] = out_re;
    out[out_idx+1] = out_im;
    out[out_deriv_x_base+0] = -out_im * k_x; // deriv_x_re
    out[out_deriv_x_base+1] =  out_re * k_x; // deriv_x_im
    out[out_deriv_z_base+0] = -out_im * k_z; // deriv_z_re
    out[out_deriv_z_base+1] =  out_re * k_z; // deriv_z_im
    real factor = (k < (real)(1e-10)) ? ((real)(0)) : ((real)(1) / k);
    out[out_disp_x_base+0] = -out_im * k_x * factor; // disp_x_re
    out[out_disp_x_base+1] =  out_re * k_x * factor; // disp_x_im
    out[out_disp_z_base+0] = -out_im * k_z * factor; // disp_z_re
    out[out_disp_z_base+1] =  out_re * k_z * factor; // disp_z_im
}
