#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace pitchlite
{
class RingBuffer
{

  public:
    int N;
    std::vector<float> buffer;

    explicit RingBuffer(int N)
        : N(N), buffer(std::vector<float>(N)), writeIndex(0)
    {
        if (N == 0 || (N & (N - 1)) != 0)
        {
            throw std::invalid_argument(
                "RingBuffer size must be a power of two");
        }
    };

    // delete the default constructor
    RingBuffer() = delete;

    float &operator[](std::size_t i) { return buffer[i]; }

    // New append method that accepts a pointer to an array of floats and its
    // length
    void append(const float *values, std::size_t length)
    {
        for (std::size_t i = 0; i < length; ++i)
        {
            buffer[writeIndex] = values[i];
            writeIndex = (writeIndex + 1) & (N - 1);
        }
    }

    float *data() { return buffer.data(); }

    void clear()
    {
        writeIndex = 0;
        std::fill(buffer.data(), buffer.data() + N, 0.0f);
    }

    int size() { return N; }

  private:
    std::size_t writeIndex;
};
} // namespace pitchlite

#endif // RINGBUFFER_HPP
