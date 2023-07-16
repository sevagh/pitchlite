// wasm_glue.cpp
#include "pitchlite.hpp"
#include "ringbuffer.hpp"
#include <cstdlib>
#include <emscripten.h>
#include <iostream>

using namespace pitchlite;

extern "C"
{
    static Mpm *mpm_big;
    static Mpm *mpm_small;
    static RingBuffer *ring_buffer;

    static int big_length;
    static int small_length;
    static int n_pitches_out;
    static int sample_rate;
    static int ring_buffer_writes = 0;

    EMSCRIPTEN_KEEPALIVE
    int pitchliteInit(int bigwin, int smallwin, int sample_rate_param)
    {
        // ensure bigwin is evenly divisible by smallwin
        if (bigwin % smallwin != 0)
        {
            fprintf(stderr, "bigwin must be evenly divisible by smallwin\n");
            exit(1);
        }
        mpm_big = new Mpm(bigwin);
        mpm_small = new Mpm(smallwin);
        ring_buffer = new RingBuffer(bigwin);
        big_length = bigwin;
        small_length = smallwin;
        n_pitches_out = 1 + (bigwin / smallwin);
        sample_rate = sample_rate_param;

        // print all the parameters
        std::cout << "big_length: " << big_length << std::endl;
        std::cout << "small_length: " << small_length << std::endl;
        std::cout << "n_pitches_out: " << n_pitches_out << std::endl;
        std::cout << "sample_rate: " << sample_rate << std::endl;

        return n_pitches_out;
    }

    EMSCRIPTEN_KEEPALIVE
    bool pitchlitePitches(const float *input_waveform, int waveform_length,
                         float *pitches_output)
    {
        ring_buffer->append(input_waveform, waveform_length);
        ring_buffer_writes += waveform_length;

        if (ring_buffer_writes < big_length)
        {
            // no pitches to compute yet
            return false;
        }
        else
        {
            // we have enough data to compute pitches

            // reset ring_buffer_writes to 0
            ring_buffer_writes -= big_length;

            // now the ring_buffer's buffer is full
            // we can get the pitch for the full buffer

            // input_waveform is the full big_length in size
            pitches_output[n_pitches_out - 1] =
                mpm_big->pitch(ring_buffer->buffer.data(), sample_rate);

            // get pitch for sub-waveforms of input_waveform divided into
            // consecutive waveforms of size small_length
            for (int i = 0; i < n_pitches_out - 1; i++)
            {
                pitches_output[i] = mpm_small->pitch(
                    ring_buffer->buffer.data() + (i * small_length),
                    sample_rate);
            }

            return true;
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void pitchliteDeinit()
    {
        delete mpm_big;
        delete mpm_small;
        delete ring_buffer;

        // Set pointers to null for safety
        mpm_big = nullptr;
        mpm_small = nullptr;
        ring_buffer = nullptr;
    }
}
