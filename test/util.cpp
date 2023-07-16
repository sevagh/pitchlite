#include "util.hpp"
#include "pitchlite.hpp"
#include <cmath>
#include <float.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

std::vector<float>
test_util::sinewave(size_t size, float frequency, int sample_rate)
{
	size_t lut_size = size / 4;

	std::vector<int> lut{};
	float *_tone_single_channel = (float *)malloc(sizeof(float) * size / 2);

	float floatf = (float)frequency;
	float delta_phi = floatf * lut_size * 1.0 / sample_rate;
	float phase = 0.0;

	for (int i = 0; i < signed(lut_size); ++i) {
		lut.push_back((int)roundf(0x7FFF * sinf(2.0 * M_PI * i / lut_size)));
	}

	float min = DBL_MAX;
	float max = -DBL_MAX;
	for (int i = 0; i < signed(size / 2); ++i) {
		int val = float(lut[(int)phase]);
		if (val > max) {
			max = val;
		}
		if (val < min) {
			min = val;
		}
		_tone_single_channel[i] = val;
		phase += delta_phi;
		if (phase >= lut_size)
			phase -= lut_size;
	}

	std::vector<float> tone_single_channel(
	    _tone_single_channel, _tone_single_channel + size / 2);

	return tone_single_channel;
}

std::vector<float>
test_util::vec_from_file(std::string path)
{
	std::vector<float> data;
	std::ifstream infile(path);

	if (infile.fail()) {
		std::cerr << "File '" << path << "' doesn't exist, exiting"
		          << std::endl;
		exit(1);
	}

	float val;
	while (infile >> val)
		data.push_back(val);

	return data;
}
