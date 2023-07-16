#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <cstddef>
#include <vector>

namespace pitchlite {
    class RingBuffer {

    public:
        int N;
        std::vector<float> buffer;

        RingBuffer(int N) : buffer(std::vector<float>(N)), writeIndex(0) {};

        // delete the default constructor
        RingBuffer() = delete;

        float &operator[](std::size_t i)
        {
            return buffer[(i + writeIndex) & (N-1)];
        }

        // New append method that accepts a pointer to an array of floats and its length
        void append(const float* values, std::size_t length)
        {
            for (std::size_t i = 0; i < length; ++i) {
                buffer[writeIndex] = values[i];
                writeIndex = (writeIndex + 1) & (N-1);
            }
        }

    private:
        std::size_t writeIndex;
    };
}

#endif // RINGBUFFER_HPP
