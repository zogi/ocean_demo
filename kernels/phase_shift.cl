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
    const int size_x = N / 2 + 1;
    const int i = global_idx & 1;              // 0..1 (real/imag)
    const int x_i = (global_idx / 2) % size_x; // 0..N/2
    const int z_i = global_idx / (2 * size_x); // 0..M-1
    const int local_idx = global_idx % LOCAL_SIZE;

    const real k_x = two_pi * x_i / Lx;
    const int z = (z_i + M / 2) % M - M / 2;
    const real k_z = two_pi * z / Lz;
    const real k = sqrt(k_x * k_x + k_z * k_z);

    const real omega = dispersion_relation(k);
    real cos_omegat;
    const real sin_omegat = sincos(omega * t, &cos_omegat);

    //const int in_stride = N + 2;
    //const int in_idx = in_stride * z_i + 2 * x_i + i;
    local real h_tilde[LOCAL_SIZE];
    h_tilde[local_idx] = in[global_idx];

    const real isign = i ? 1 : -1;
    const int out_row_stride = N + 2;
    const int out_buf_stride = out_row_stride * M;
    const int out_idx = out_row_stride * z_i + 2 * x_i;
    local real h_tilde_shifted[LOCAL_SIZE];
    // h_tilde_shifted = h_tilde * exp(i*omaga*t) <- i is the imaginary unit here
    h_tilde_shifted[local_idx] = h_tilde[local_idx] * cos_omegat + isign * h_tilde[local_idx^1] * sin_omegat;
    out[global_idx] = h_tilde_shifted[local_idx];

    const int out_deriv_x_idx =    out_buf_stride + global_idx;
    const int out_deriv_z_idx =  2*out_buf_stride + global_idx;
    const int out_choppy_x_idx = 3*out_buf_stride + global_idx;
    const int out_choppy_z_idx = 4*out_buf_stride + global_idx;

    // dh_dx_tilde_shifted = i * k_x * h_tilde_shifted
    // dh_dz_tilde_shifted = i * k_z * h_tilde_shifted
    local real dh_dx_tilde_shifted[LOCAL_SIZE];
    local real dh_dz_tilde_shifted[LOCAL_SIZE];
    dh_dx_tilde_shifted[local_idx] = isign * k_x * h_tilde_shifted[local_idx^1];
    dh_dz_tilde_shifted[local_idx] = isign * k_z * h_tilde_shifted[local_idx^1];
    out[out_deriv_x_idx] = dh_dx_tilde_shifted[local_idx];
    out[out_deriv_z_idx] = dh_dz_tilde_shifted[local_idx];

    // choppy_x_tilde_shifted = i * k_x / k * h_tilde_shifted = dh_dx_tilde_shifted / k
    // choppy_z_tilde_shifted = i * k_z / k * h_tilde_shifted = dh_dz_tilde_shifted / k
    real factor = (k < (real)(1e-10)) ? ((real)(0)) : ((real)(1) / k);
    out[out_choppy_x_idx] = dh_dx_tilde_shifted[local_idx] * factor;
    out[out_choppy_z_idx] = dh_dz_tilde_shifted[local_idx] * factor;
}
