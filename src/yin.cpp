#include "pitchlite.hpp"
#include <algorithm>
#include <map>
#include <tuple>
#include <vector>

namespace pitchlite
{
float Yin::YinThreshold = 0.2;
}

static int absolute_threshold(const std::vector<float> &yin_buffer,
                              float yin_threshold)
{
    ssize_t size = yin_buffer.size();
    int tau;
    for (tau = 2; tau < size; tau++)
    {
        if (yin_buffer[tau] < yin_threshold)
        {
            while (tau + 1 < size && yin_buffer[tau + 1] < yin_buffer[tau])
            {
                tau++;
            }
            break;
        }
    }
    return (tau == size || yin_buffer[tau] >= yin_threshold) ? -1 : tau;
}

static void difference(const float *audio_buffer, pitchlite::Yin *ya)
{
    pitchlite::acorr_r(audio_buffer, ya);

    for (int tau = 0; tau < ya->N / 2; tau++)
        ya->yin_buffer[tau] =
            ya->out_real[0] + ya->out_real[1] - 2 * ya->out_real[tau];
}

static void
cumulative_mean_normalized_difference(std::vector<float> &yin_buffer)
{
    double running_sum = 0.0f;

    yin_buffer[0] = 1;

    for (int tau = 1; tau < signed(yin_buffer.size()); tau++)
    {
        running_sum += yin_buffer[tau];
        yin_buffer[tau] *= tau / running_sum;
    }
}

float pitchlite::Yin::pitch(const float *audio_buffer)
{
    int tau_estimate;

    difference(audio_buffer, this);

    cumulative_mean_normalized_difference(this->yin_buffer);
    tau_estimate = absolute_threshold(this->yin_buffer, this->YinThreshold);

    auto ret = (tau_estimate != -1)
                   ? this->sample_rate /
                         std::get<0>(pitchlite::parabolic_interpolation(
                             this->yin_buffer, tau_estimate))
                   : -1;

    this->clear();
    return ret;
}
