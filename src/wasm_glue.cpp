// wasm_glue.cpp
#include "pitchlite.hpp"
#include <cstdlib>
#include <emscripten.h>
#include <iostream>

using namespace pitchlite;

extern "C"
{
    static bool yin_used;
    static Mpm *mpm_big;
    static Mpm *mpm_small;
    static Yin *yin_big;
    static Yin *yin_small;

    static int big_length;
    static int small_length;
    static int n_pitches_out;

    EMSCRIPTEN_KEEPALIVE
    int pitchliteInit(int bigwin, int smallwin, int sample_rate, bool use_yin,
                      float mpm_lower_pitch_cutoff)
    {
        yin_used = use_yin;

        // ensure bigwin is evenly divisible by smallwin
        if (bigwin % smallwin != 0)
        {
            fprintf(stderr, "bigwin must be evenly divisible by smallwin\n");
            exit(1);
        }

        if (use_yin)
        {
            yin_big = new Yin(bigwin, sample_rate);
            yin_small = new Yin(smallwin, sample_rate);
        }
        else
        {
            mpm_big = new Mpm(bigwin, sample_rate);
            mpm_small = new Mpm(smallwin, sample_rate);
        }

        big_length = bigwin;
        small_length = smallwin;
        n_pitches_out = 1 + (bigwin / smallwin);

        // print all the parameters
        std::cout << "yin_used: " << std::boolalpha << yin_used << std::endl;
        std::cout << "mpm lower pitch cutoff: " << mpm_lower_pitch_cutoff
                  << std::endl;
        std::cout << "big_length: " << big_length << std::endl;
        std::cout << "small_length: " << small_length << std::endl;
        std::cout << "n_pitches_out: " << n_pitches_out << std::endl;
        std::cout << "sample_rate: " << sample_rate << std::endl;

        return n_pitches_out;
    }

    EMSCRIPTEN_KEEPALIVE
    void pitchlitePitches(const float *input_waveform, float *pitches_output)
    {
        // input_waveform is the full big_length in size
        pitches_output[n_pitches_out - 1] =
            yin_used ? yin_big->pitch(input_waveform)
                     : mpm_big->pitch(input_waveform);

        // print the pitch of the full buffer
        if (pitches_output[n_pitches_out - 1] != -1.0)
        {
            std::cout << "pitch: " << pitches_output[n_pitches_out - 1]
                      << std::endl;
        }

        // get pitch for sub-waveforms of input_waveform divided into
        // consecutive waveforms of size small_length
        for (int i = 0; i < n_pitches_out - 1; ++i)
        {
            pitches_output[i] =
                yin_used
                    ? yin_small->pitch(input_waveform + (i * small_length))
                    : mpm_small->pitch(input_waveform + (i * small_length));
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void pitchliteDeinit()
    {
        // Free memory and set pointers to null for safety
        if (yin_used)
        {
            delete yin_big;
            delete yin_small;
            yin_big = nullptr;
            yin_small = nullptr;
        }
        else
        {
            delete mpm_big;
            delete mpm_small;
            mpm_big = nullptr;
            mpm_small = nullptr;
        }
    }
}
