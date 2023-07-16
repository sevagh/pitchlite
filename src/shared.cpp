#include "pitchlite.hpp"
#include <algorithm>
#include <kiss_fftr.h>
#include <numeric>
#include <vector>

static kiss_fft_cpx complex_mult(const kiss_fft_cpx &a, const kiss_fft_cpx &b)
{
    kiss_fft_cpx c;

    // real_part = a.real*b.real - a.imag*b.imag
    c.r = a.r * b.r - a.i * b.i;

    // imag_part = a.real*b.imag + b.real*a.imag
    c.i = a.r * b.i + b.r * a.i;

    return c;
}

static kiss_fft_cpx complex_conj(const kiss_fft_cpx &a)
{
    kiss_fft_cpx c;

    c.r = a.r;
    c.i = -a.i;

    return c;
}

void pitchlite::acorr_r(const float *audio_buffer, pitchlite::PitchBase *ba)
{
    // forward real FFT
    kiss_fftr(ba->fft_forward, audio_buffer, ba->out_im.data());

    // for each bin in the first half (plus DC and Nyquist bins)
    for (int i = 0; i < ba->N / 2 + 1; ++i)
    {
        // multiply by complex conjugate
        ba->out_im[i] =
            complex_mult(ba->out_im[i], complex_conj(ba->out_im[i]));

        // scale
        kiss_fft_cpx scale = {1.0f / (float)(ba->N * 2), 0.0f};
        ba->out_im[i] = complex_mult(ba->out_im[i], scale);
    }

    // inverse real FFT
    kiss_fftri(ba->fft_backward, ba->out_im.data(), ba->out_real.data());
}

std::pair<float, float>
pitchlite::parabolic_interpolation(const std::vector<float> &array, int x_)
{
    int x_adjusted;
    float x = (float)x_;

    if (x < 1)
    {
        x_adjusted = (array[x] <= array[x + 1]) ? x : x + 1;
    }
    else if (x > signed(array.size()) - 1)
    {
        x_adjusted = (array[x] <= array[x - 1]) ? x : x - 1;
    }
    else
    {
        float den = array[x + 1] + array[x - 1] - 2 * array[x];
        float delta = array[x - 1] - array[x + 1];
        return (!den) ? std::make_pair(x, array[x])
                      : std::make_pair(x + delta / (2 * den),
                                       array[x] - delta * delta / (8 * den));
    }
    return std::make_pair(x_adjusted, array[x_adjusted]);
}
