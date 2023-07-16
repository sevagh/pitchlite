#include "pitchlite.hpp"
#include "util.hpp"
#include <gtest/gtest.h>

TEST(YinInstrumentTest, Violin_A4_44100)
{
	auto data = test_util::vec_from_file("./test/samples/A4_44100_violin.txt");
        auto yin = pitchlite::Yin(4096);
	float pitch = yin.pitch(data.data(), 44100);
	float expected = 440.0;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(YinInstrumentTest, Piano_B4_44100)
{
	auto data = test_util::vec_from_file("./test/samples/B4_44100_piano.txt");
        auto yin = pitchlite::Yin(4096);
	float pitch = yin.pitch(data.data(), 44100);
	float expected = 493.9;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(YinInstrumentTest, Piano_D4_44100)
{
	auto data = test_util::vec_from_file("./test/samples/D4_44100_piano.txt");
        auto yin = pitchlite::Yin(4096);
	float pitch = yin.pitch(data.data(), 44100);
	float expected = 293.7;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(YinInstrumentTest, Acoustic_E2_44100)
{
	auto data =
	    test_util::vec_from_file("./test/samples/E2_44100_acousticguitar.txt");
        auto yin = pitchlite::Yin(4096);
	float pitch = yin.pitch(data.data(), 44100);
        float expected = 82.41;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(YinInstrumentTest, Classical_FSharp4_48000)
{
	auto data = test_util::vec_from_file(
	    "./test/samples/F-4_48000_classicalguitar.txt");
        auto yin = pitchlite::Yin(4096);
	float pitch = yin.pitch(data.data(), 44100);
	float expected = 370.0;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(MpmInstrumentTest, Violin_A4_44100)
{
	auto data = test_util::vec_from_file("./test/samples/A4_44100_violin.txt");
        auto mpm = pitchlite::Mpm(4096);
	float pitch = mpm.pitch(data.data(), 44100);
	float expected = 440.0;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(MpmInstrumentTest, Piano_B4_44100)
{
	auto data = test_util::vec_from_file("./test/samples/B4_44100_piano.txt");
        auto mpm = pitchlite::Mpm(4096);
	float pitch = mpm.pitch(data.data(), 44100);
	float expected = 493.9;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(MpmInstrumentTest, Piano_D4_44100)
{
	auto data = test_util::vec_from_file("./test/samples/D4_44100_piano.txt");
        auto mpm = pitchlite::Mpm(4096);
	float pitch = mpm.pitch(data.data(), 44100);
	float expected = 293.7;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(MpmInstrumentTest, Acoustic_E2_44100)
{
	auto data =
	    test_util::vec_from_file("./test/samples/E2_44100_acousticguitar.txt");
        auto mpm = pitchlite::Mpm(4096);
	float pitch = mpm.pitch(data.data(), 44100);
	float expected = 82.41;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}

TEST(MpmInstrumentTest, Classical_FSharp4_48000)
{
	auto data = test_util::vec_from_file(
	    "./test/samples/F-4_48000_classicalguitar.txt");
        auto mpm = pitchlite::Mpm(4096);
	float pitch = mpm.pitch(data.data(), 44100);
	float expected = 370.0;
	EXPECT_NEAR(expected, pitch, 0.01 * expected);
}
