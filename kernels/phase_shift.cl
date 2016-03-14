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

#define LOCAL_SIZE 64

kernel void phase_shift(global const real *in, real Lx, real Lz, int N, int M, real t, global real *out)
{
    const int global_idx = get_global_id(0);
    const int local_idx = global_idx % LOCAL_SIZE;
    const int size_x = N / 2 + 1;
    const int i = global_idx & 1;              // 0..1 (real/imag)
    const int x_i = (global_idx / 2) % size_x; // 0..N/2
    const int z_i = global_idx / (2 * size_x); // 0..M-1

    const size_t out_row_stride = N + 2;
    const size_t out_buf_stride = out_row_stride * M;
    const size_t out_idx = out_row_stride * z_i + 2 * x_i;

    // Pointers to the Fourier transform of the displacement field.
    global real *dx_out =     out + 0 * out_buf_stride;
    global real *dy_out =     out + 1 * out_buf_stride;
    global real *dz_out =     out + 2 * out_buf_stride;

    // Pointers to the Fourier transform of the jacobian of the displacement field.
    global real *ddx_du_out = out + 3 * out_buf_stride;
    global real *ddx_dv_out = out + 4 * out_buf_stride;
    global real *ddy_du_out = out + 5 * out_buf_stride;
    global real *ddy_dv_out = out + 6 * out_buf_stride;
    global real *ddz_du_out = out + 7 * out_buf_stride;
    global real *ddz_dv_out = out + 8 * out_buf_stride;

    const real k_x = two_pi * x_i / Lx;
    const int z = (z_i + M / 2) % M - M / 2;
    const real k_z = two_pi * z / Lz;
    const real k = sqrt(k_x * k_x + k_z * k_z);

    const real omega = dispersion_relation(k);
    real cos_omegat;
    const real sin_omegat = sincos(omega * t, &cos_omegat);

    // Fourier transform of the height (y displacement) at t=0.
    local real h_tilde[LOCAL_SIZE];
    h_tilde[local_idx] = in[global_idx];

    const real isign = i ? 1 : -1;

    // h_tilde_shifted = h_tilde * exp(i*omaga*t) <- i is the imaginary unit here
    local real h_tilde_shifted[LOCAL_SIZE];
    h_tilde_shifted[local_idx] = h_tilde[local_idx] * cos_omegat + isign * h_tilde[local_idx^1] * sin_omegat;

    // dh_du_tilde_shifted = i * k_x * h_tilde_shifted
    // dh_dv_tilde_shifted = i * k_z * h_tilde_shifted
    local real dh_du_tilde_shifted[LOCAL_SIZE];
    local real dh_dv_tilde_shifted[LOCAL_SIZE];
    dh_du_tilde_shifted[local_idx] = isign * k_x * h_tilde_shifted[local_idx^1];
    dh_dv_tilde_shifted[local_idx] = isign * k_z * h_tilde_shifted[local_idx^1];

    // Vertical displacement.
    dy_out[global_idx] = h_tilde_shifted[local_idx];

    // Horizontal displacement (choppy waves).
    real factor = (k < (real)(1e-10)) ? ((real)(0)) : ((real)(1) / k);
    dx_out[global_idx] = dh_du_tilde_shifted[local_idx] * factor;
    dz_out[global_idx] = dh_dv_tilde_shifted[local_idx] * factor;

    // Gradient of vertical displacement.
    ddy_du_out[global_idx] = dh_du_tilde_shifted[local_idx];
    ddy_dv_out[global_idx] = dh_dv_tilde_shifted[local_idx];

    // Jacobian of horizontal displacement.
    ddx_du_out[global_idx] = k_x * k_x * factor * h_tilde_shifted[local_idx];
    ddx_dv_out[global_idx] = k_x * k_z * factor * h_tilde_shifted[local_idx];
    ddz_du_out[global_idx] = k_z * k_x * factor * h_tilde_shifted[local_idx];
    ddz_dv_out[global_idx] = k_z * k_z * factor * h_tilde_shifted[local_idx];
}
