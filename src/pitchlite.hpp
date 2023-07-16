#ifndef PITCHLITE_H
#define PITCHLITE_H

#include <kiss_fftr.h>
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

class PitchBase
{
  public:
    long N;

    kiss_fftr_cfg fft_forward;
    kiss_fftr_cfg fft_backward;

    // intermediate buffer for kissfft
    std::vector<kiss_fft_cpx> out_im;
    std::vector<float> out_real;

    int sample_rate;

    PitchBase(long audio_buffer_size, int sample_rate)
        : N(audio_buffer_size), out_im(std::vector<kiss_fft_cpx>(N / 2 + 1)),
          out_real(std::vector<float>(N)), sample_rate(sample_rate)
    {
        if (N == 0)
        {
            throw std::bad_alloc();
        }

        // need a kissfftr object
        fft_forward = kiss_fftr_alloc(N, 0, nullptr, nullptr);
        fft_backward = kiss_fftr_alloc(N, 1, nullptr, nullptr);
    }

    ~PitchBase()
    {
        free(fft_forward);
        free(fft_backward);
    }

    virtual float pitch(const float *) = 0;

  protected:
    void clear()
    {
        std::fill(out_im.begin(), out_im.end(), kiss_fft_cpx{0.0, 0.0});
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
class Mpm : public PitchBase
{
  public:
    // define static fixed constants
    static float MpmCutoff;
    static float MpmSmallCutoff;
    static float MpmLowerPitchCutoff;

    Mpm(long audio_buffer_size, int sample_rate)
        : PitchBase(audio_buffer_size, sample_rate){};

    float pitch(const float *) override;

    static void setMpmCutoff(float value) { MpmCutoff = value; }
    static void setMpmSmallCutoff(float value) { MpmSmallCutoff = value; }
    static void setMpmLowerPitchCutoff(float value)
    {
        MpmLowerPitchCutoff = value;
    }

    virtual ~Mpm()
    {
        // do any cleanup that is specific to Mpm class
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
class Yin : public PitchBase
{
  public:
    static float YinThreshold;
    std::vector<float> yin_buffer;

    Yin(long audio_buffer_size, int sample_rate)
        : PitchBase(audio_buffer_size, sample_rate),
          yin_buffer(std::vector<float>(audio_buffer_size / 2))
    {
        if (audio_buffer_size / 2 == 0)
        {
            throw std::bad_alloc();
        }
    }

    float pitch(const float *) override;

    static void setYinThreshold(float value) { YinThreshold = value; }

    virtual ~Yin()
    {
        // do any cleanup that is specific to Yin class
    }
};

std::pair<float, float> parabolic_interpolation(const std::vector<float> &,
                                                int);

void acorr_r(const float *, PitchBase *);
} // namespace pitchlite

#endif /* PITCHLITE */
