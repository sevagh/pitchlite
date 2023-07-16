#include "ringbuffer.hpp"
#include "gtest/gtest.h"

using namespace pitchlite;

class RingBufferTest : public ::testing::Test
{
  protected:
    // You can remove any or all of the following functions if they're not
    // needed

    RingBufferTest()
    {
        // You can do set-up work for each test here.
    }

    ~RingBufferTest() override
    {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    void SetUp() override
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }
};

TEST_F(RingBufferTest, TestAppend)
{
    int size = 8;
    RingBuffer rb(size);
    float data[] = {1.0, 2.0, 3.0, 4.0, 5.0};

    rb.append(data, 5);

    EXPECT_FLOAT_EQ(rb[0], 1.0);
    EXPECT_FLOAT_EQ(rb[1], 2.0);
    EXPECT_FLOAT_EQ(rb[2], 3.0);
    EXPECT_FLOAT_EQ(rb[3], 4.0);
    EXPECT_FLOAT_EQ(rb[4], 5.0);
    EXPECT_FLOAT_EQ(rb[5], 0.0);
    EXPECT_FLOAT_EQ(rb[6], 0.0);
    EXPECT_FLOAT_EQ(rb[7], 0.0);
}

TEST_F(RingBufferTest, TestOverwrite)
{
    int size = 8;
    RingBuffer rb(size);
    float data1[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    float data2[] = {9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};

    rb.append(data1, 8);
    rb.append(data2, 8);

    EXPECT_FLOAT_EQ(rb[0], 9.0);
    EXPECT_FLOAT_EQ(rb[1], 10.0);
    EXPECT_FLOAT_EQ(rb[2], 11.0);
    EXPECT_FLOAT_EQ(rb[3], 12.0);
    EXPECT_FLOAT_EQ(rb[4], 13.0);
    EXPECT_FLOAT_EQ(rb[5], 14.0);
    EXPECT_FLOAT_EQ(rb[6], 15.0);
    EXPECT_FLOAT_EQ(rb[7], 16.0);
}

TEST_F(RingBufferTest, TestWasmUsage)
{
    int bufferSize = 4096;
    int chunkSize = 128;
    int chunksCount = bufferSize / chunkSize;
    float chunkData[chunkSize];
    float outputBuffer[bufferSize];

    // Initialize the ring buffer
    RingBuffer rb(bufferSize);

    // Simulate the receiving of consecutive chunks of 128 samples
    for (int i = 0; i < chunksCount; ++i)
    {
        // Simulate the creation of the chunk
        for (int j = 0; j < chunkSize; ++j)
        {
            chunkData[j] = static_cast<float>(j + i * chunkSize);
        }

        // Append the chunk to the ring buffer
        rb.append(chunkData, chunkSize);
    }

    // Now, the ring buffer should be full.
    // Read the entire buffer
    for (int i = 0; i < bufferSize; ++i)
    {
        outputBuffer[i] = rb[i];
    }

    // Verify that the data in the output buffer matches the original data
    for (int i = 0; i < bufferSize; ++i)
    {
        EXPECT_FLOAT_EQ(outputBuffer[i], static_cast<float>(i));
    }
}

TEST_F(RingBufferTest, TestWasmUsageWithOverwrite)
{
    int bufferSize = 4096;
    int chunkSize = 128;
    int chunksCount = bufferSize / chunkSize;
    float chunkData[chunkSize];
    float outputBuffer[bufferSize];

    // Initialize the ring buffer
    RingBuffer rb(bufferSize);

    // Simulate the receiving of consecutive chunks of 128 samples
    for (int i = 0; i < chunksCount; ++i)
    {
        // Simulate the creation of the chunk
        for (int j = 0; j < chunkSize; ++j)
        {
            chunkData[j] = static_cast<float>(j + i * chunkSize);
        }

        // Append the chunk to the ring buffer
        rb.append(chunkData, chunkSize);
    }

    // Now, the ring buffer should be full.
    // Read the entire buffer
    for (int i = 0; i < bufferSize; ++i)
    {
        outputBuffer[i] = rb[i];
    }

    // Verify that the data in the output buffer matches the original data
    for (int i = 0; i < bufferSize; ++i)
    {
        EXPECT_FLOAT_EQ(outputBuffer[i], static_cast<float>(i));
    }

    // Now, let's overwrite the buffer with new data.
    for (int i = 0; i < chunksCount; ++i)
    {
        // Simulate the creation of the chunk
        for (int j = 0; j < chunkSize; ++j)
        {
            chunkData[j] = static_cast<float>(j + i * chunkSize + bufferSize);
        }

        // Append the chunk to the ring buffer
        rb.append(chunkData, chunkSize);
    }

    // Now, the ring buffer should have been overwritten with the new data.
    // Read the entire buffer again
    for (int i = 0; i < bufferSize; ++i)
    {
        outputBuffer[i] = rb[i];
    }

    // Verify that the data in the output buffer matches the new data
    for (int i = 0; i < bufferSize; ++i)
    {
        EXPECT_FLOAT_EQ(outputBuffer[i], static_cast<float>(i + bufferSize));
    }
}
