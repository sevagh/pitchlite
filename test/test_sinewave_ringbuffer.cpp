#include "pitchlite.hpp"
#include "ringbuffer.hpp"
#include "util.hpp"
#include <gtest/gtest.h>

class MpmRingbufferSinewaveTest : public testing::TestWithParam<float>
{
};

class YinRingbufferSinewaveTest : public testing::TestWithParam<float>
{
};

TEST_P(MpmRingbufferSinewaveTest, GetFreqManualAlloc)
{
    double freq = GetParam();
    auto data = test_util::sinewave(8192, freq, 48000);
    pitchlite::RingBuffer rb(data.size());

    rb.append(data.data(), data.size());

    pitchlite::Mpm ma(rb.size(), 48000);
    double pitch = ma.pitch(rb.data());
    EXPECT_NEAR(freq, pitch, 0.01 * freq);
}

TEST_P(YinRingbufferSinewaveTest, GetFreqManualAlloc)
{
    double freq = GetParam();
    auto data = test_util::sinewave(8192, freq, 48000);
    pitchlite::RingBuffer rb(data.size());

    rb.append(data.data(), data.size());

    pitchlite::Yin ya(rb.size(), 48000);
    double pitch = ya.pitch(rb.data());
    EXPECT_NEAR(freq, pitch, 0.01 * freq);
}

TEST(MpmRingbufferSinewaveTestManualAlloc, OneAllocMultipleFreq)
{
    auto data1 = test_util::sinewave(8192, 150.0, 48000);
    auto data2 = test_util::sinewave(8192, 250.0, 48000);
    auto data3 = test_util::sinewave(8192, 350.0, 48000);

    pitchlite::RingBuffer rb(data1.size());

    rb.append(data1.data(), data1.size());
    pitchlite::Mpm ma(rb.size(), 48000);
    double pitch1 = ma.pitch(rb.data());

    rb.clear();
    rb.append(data2.data(), data2.size());
    double pitch2 = ma.pitch(rb.data());

    rb.clear();
    rb.append(data3.data(), data3.size());
    double pitch3 = ma.pitch(rb.data());

    EXPECT_NEAR(150.0, pitch1, 0.01 * 150.0);
    EXPECT_NEAR(250.0, pitch2, 0.01 * 250.0);
    EXPECT_NEAR(350.0, pitch3, 0.01 * 350.0);
}

TEST(YinRingbufferSinewaveTestManualAlloc, OneAllocMultipleFreq)
{
    auto data1 = test_util::sinewave(8192, 150.0, 48000);
    auto data2 = test_util::sinewave(8192, 250.0, 48000);
    auto data3 = test_util::sinewave(8192, 350.0, 48000);

    pitchlite::RingBuffer rb(data1.size());

    rb.append(data1.data(), data1.size());
    pitchlite::Yin ya(rb.size(), 48000);
    double pitch1 = ya.pitch(rb.data());

    rb.clear();
    rb.append(data2.data(), data2.size());
    double pitch2 = ya.pitch(rb.data());

    rb.clear();
    rb.append(data3.data(), data3.size());
    double pitch3 = ya.pitch(rb.data());

    EXPECT_NEAR(150.0, pitch1, 0.01 * 150.0);
    EXPECT_NEAR(250.0, pitch2, 0.01 * 250.0);
    EXPECT_NEAR(350.0, pitch3, 0.01 * 350.0);
}

// no 77.0hz for mpm because it can't
INSTANTIATE_TEST_CASE_P(MpmRingbufferSinewave, MpmRingbufferSinewaveTest,
                        ::testing::Values(100.0, 233.0, 298.0, 1583.0, 3398.0,
                                          4200.0));

INSTANTIATE_TEST_CASE_P(YinRingbufferSinewave, YinRingbufferSinewaveTest,
                        ::testing::Values(77.0, 100.0, 233.0, 298.0, 1583.0,
                                          3398.0, 4200.0));
