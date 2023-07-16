#ifndef PITCHLITE_H
#define PITCHLITE_H

#include <kiss_fft.h>
#include <stdexcept>
#include <vector>

// i'd rather use std::vector<float> than allocate kiss_fft_scalar
// this static_assert will help
static_assert(std::is_same<kiss_fft_scalar, float>::value,
              "kiss_fft_scalar must be a float");

/*
 * This namespace is useful for repeated calls to pitch for the same size of
 * buffer.
 *
 * It contains the classes Yin and Mpm which contain the allocated buffers
 * and each implement a `pitch(data, sample_rate)`
 */
namespace pitchlite
{

class BaseAlloc
{
  public:
    long N;
    // 2 intermediate buffers for kissfft
    std::vector<kiss_fft_cpx> out_im_1;
    std::vector<kiss_fft_cpx> out_im_2;
    std::vector<float> out_real;
    kiss_fft_cfg fft_forward;
    kiss_fft_cfg fft_backward;

    BaseAlloc(long audio_buffer_size)
        : N(audio_buffer_size),
        out_im_1(std::vector<kiss_fft_cpx>(2 * N)),
        out_im_2(std::vector<kiss_fft_cpx>(2 * N)),
        out_real(std::vector<float>(N))
    {
        if (N == 0)
        {
            throw std::bad_alloc();
        }

        // need a kissfftr object
        fft_forward = kiss_fft_alloc(N, 0, nullptr, nullptr);
        fft_backward = kiss_fft_alloc(N, 1, nullptr, nullptr);
    }

    ~BaseAlloc()
    {
        free(fft_forward);
        free(fft_backward);
    }

  protected:
    void clear()
    {
        std::fill(out_im_1.begin(), out_im_1.end(), kiss_fft_cpx{0.0, 0.0});
        std::fill(out_im_2.begin(), out_im_2.end(), kiss_fft_cpx{0.0, 0.0});
    }
};

/*
 * Allocate the buffers for MPM for re-use.
 * Intended for multiple consistently-sized audio buffers.
 *
 * Usage: pitchlite::Mpm ma(1024)
 *
 * It will throw std::bad_alloc for invalid sizes (<1)
 */
class Mpm : public BaseAlloc
{
  public:
    // define static fixed constants
    static float MpmCutoff;
    static float MpmSmallCutoff;
    static float MpmLowerPitchCutoff;

    Mpm(long audio_buffer_size) : BaseAlloc(audio_buffer_size){};

    float pitch(const float *, int);

    static void setMpmCutoff(float value) { MpmCutoff = value; }
    static void setMpmSmallCutoff(float value) { MpmSmallCutoff = value; }
    static void setMpmLowerPitchCutoff(float value)
    {
        MpmLowerPitchCutoff = value;
    }
};

/*
 * Allocate the buffers for YIN for re-use.
 * Intended for multiple consistently-sized audio buffers.
 *
 * Usage: pitchlite::Yin ya(1024)
 *
 * It will throw std::bad_alloc for invalid sizes (<2)
 */
class Yin : public BaseAlloc
{
  public:
    static float YinThreshold;
    std::vector<float> yin_buffer;

    Yin(long audio_buffer_size)
        : BaseAlloc(audio_buffer_size),
          yin_buffer(std::vector<float>(audio_buffer_size / 2))
    {
        if (audio_buffer_size / 2 == 0)
        {
            throw std::bad_alloc();
        }
    }

    float pitch(const float *, int);

    static void setYinThreshold(float value) { YinThreshold = value; }
};

std::pair<float, float> parabolic_interpolation(const std::vector<float> &,
                                                int);

void acorr_r(const float *, BaseAlloc *);
} // namespace pitchlite

#endif /* PITCHLITE */
