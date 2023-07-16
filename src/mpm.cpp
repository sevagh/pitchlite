#include "pitchlite.hpp"
#include <algorithm>
#include <float.h>
#include <map>
#include <numeric>
#include <vector>

namespace pitchlite
{
float Mpm::MpmCutoff = 0.93;
float Mpm::MpmSmallCutoff = 0.5;

// 80 Hz for guitar
float Mpm::MpmLowerPitchCutoff = 80.0;
} // namespace pitchlite

static std::vector<int> peak_picking(const std::vector<float> &nsdf)
{
    std::vector<int> max_positions{};
    int pos = 0;
    int cur_max_pos = 0;
    ssize_t size = nsdf.size();

    while (pos < (size - 1) / 3 && nsdf[pos] > 0)
        pos++;
    while (pos < size - 1 && nsdf[pos] <= 0.0)
        pos++;

    if (pos == 0)
        pos = 1;

    while (pos < size - 1)
    {
        if (nsdf[pos] > nsdf[pos - 1] && nsdf[pos] >= nsdf[pos + 1] &&
            (cur_max_pos == 0 || nsdf[pos] > nsdf[cur_max_pos]))
        {
            cur_max_pos = pos;
        }
        pos++;
        if (pos < size - 1 && nsdf[pos] <= 0)
        {
            if (cur_max_pos > 0)
            {
                max_positions.push_back(cur_max_pos);
                cur_max_pos = 0;
            }
            while (pos < size - 1 && nsdf[pos] <= 0.0)
            {
                pos++;
            }
        }
    }
    if (cur_max_pos > 0)
    {
        max_positions.push_back(cur_max_pos);
    }
    return max_positions;
}

float pitchlite::Mpm::pitch(const float *audio_buffer)
{
    pitchlite::acorr_r(audio_buffer, this);

    std::vector<int> max_positions = peak_picking(this->out_real);
    std::vector<std::pair<float, float>> estimates;

    float highest_amplitude = -DBL_MAX;

    for (int i : max_positions)
    {
        highest_amplitude = std::max(highest_amplitude, this->out_real[i]);
        if (this->out_real[i] > this->MpmSmallCutoff)
        {
            auto x = pitchlite::parabolic_interpolation(this->out_real, i);
            estimates.push_back(x);
            highest_amplitude = std::max(highest_amplitude, std::get<1>(x));
        }
    }

    if (estimates.empty())
        return -1;

    float actual_cutoff = this->MpmCutoff * highest_amplitude;
    float period = 0;

    for (auto i : estimates)
    {
        if (std::get<1>(i) >= actual_cutoff)
        {
            period = std::get<0>(i);
            break;
        }
    }

    float pitch_estimate = (this->sample_rate / period);

    this->clear();

    return (pitch_estimate > this->MpmLowerPitchCutoff) ? pitch_estimate : -1;
}
