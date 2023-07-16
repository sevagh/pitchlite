#include "pitchlite.hpp"
#include "util.hpp"
#include <gtest/gtest.h>

class MpmSinewaveTest : public testing::TestWithParam<float>
{
};

class YinSinewaveTest : public testing::TestWithParam<float>
{
};

TEST_P(MpmSinewaveTest, GetFreqManualAlloc)
{
    double freq = GetParam();
    auto data = test_util::sinewave(8192, freq, 48000);
    pitchlite::Mpm ma(data.size(), 48000);
    double pitch = ma.pitch(data.data());
    EXPECT_NEAR(freq, pitch, 0.01 * freq);
}

TEST_P(YinSinewaveTest, GetFreqManualAlloc)
{
    double freq = GetParam();
    auto data = test_util::sinewave(8192, freq, 48000);
    pitchlite::Yin ya(data.size(), 48000);
    double pitch = ya.pitch(data.data());
    EXPECT_NEAR(freq, pitch, 0.01 * freq);
}

TEST(MpmSinewaveTestManualAlloc, OneAllocMultipleFreq)
{
    auto data1 = test_util::sinewave(8192, 150.0, 48000);
    auto data2 = test_util::sinewave(8192, 250.0, 48000);
    auto data3 = test_util::sinewave(8192, 350.0, 48000);

    pitchlite::Mpm ma(data1.size(), 48000);

    double pitch1 = ma.pitch(data1.data());
    double pitch2 = ma.pitch(data2.data());
    double pitch3 = ma.pitch(data3.data());

    EXPECT_NEAR(150.0, pitch1, 0.01 * 150.0);
    EXPECT_NEAR(250.0, pitch2, 0.01 * 250.0);
    EXPECT_NEAR(350.0, pitch3, 0.01 * 350.0);
}

TEST(YinSinewaveTestManualAlloc, OneAllocMultipleFreq)
{
    auto data1 = test_util::sinewave(8192, 150.0, 48000);
    auto data2 = test_util::sinewave(8192, 250.0, 48000);
    auto data3 = test_util::sinewave(8192, 350.0, 48000);

    pitchlite::Yin ya(data1.size(), 48000);

    double pitch1 = ya.pitch(data1.data());
    double pitch2 = ya.pitch(data2.data());
    double pitch3 = ya.pitch(data3.data());

    EXPECT_NEAR(150.0, pitch1, 0.01 * 150.0);
    EXPECT_NEAR(250.0, pitch2, 0.01 * 250.0);
    EXPECT_NEAR(350.0, pitch3, 0.01 * 350.0);
}

// no 77.0hz for mpm because it can't
INSTANTIATE_TEST_CASE_P(MpmSinewave, MpmSinewaveTest,
                        ::testing::Values(100.0, 233.0, 298.0, 1583.0, 3398.0,
                                          4200.0));

INSTANTIATE_TEST_CASE_P(YinSinewave, YinSinewaveTest,
                        ::testing::Values(77.0, 100.0, 233.0, 298.0, 1583.0,
                                          3398.0, 4200.0));
