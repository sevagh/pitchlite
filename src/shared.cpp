#include "pitchlite.hpp"
#include <algorithm>
#include <kiss_fft.h>
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

void pitchlite::acorr_r(const float *audio_buffer, pitchlite::BaseAlloc *ba)
{
    std::transform(audio_buffer, audio_buffer + ba->N,
        ba->out_im_1.begin(), [](float x) -> kiss_fft_cpx  {
                return {x, 0.0f};
        });

    kiss_fft(ba->fft_forward, ba->out_im_1.data(), ba->out_im_2.data());

    kiss_fft_cpx scale = {1.0f / (float)(ba->N * 2),
                          0.0f};
    for (int i = 0; i < ba->N; ++i)
    {
        // ba->out_im[i] *= std::conj(ba->out_im[i]) * scale;
        ba->out_im_2[i] = complex_mult(
            ba->out_im_2[i], complex_mult(complex_conj(ba->out_im_2[i]), scale));
    }

    kiss_fft(ba->fft_backward, ba->out_im_2.data(), ba->out_im_1.data());

    std::transform(ba->out_im_1.begin(), ba->out_im_1.begin() + ba->N,
        ba->out_real.begin(),
        [](kiss_fft_cpx cplx) -> float { return cplx.r; });
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
