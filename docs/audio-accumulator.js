class AudioAccumulator extends AudioWorkletProcessor {
    process(inputs, outputs, parameters) {
        // Get the input and output
        const input = inputs[0];
        const output = outputs[0];

        // For each channel (assuming stereo input and output)
        for (let channel = 0; channel < input.length; ++channel) {
            // Directly copy input to output
            output[channel].set(input[channel]);
        }

        let inputLeft = input[0];
        let inputRight = input[1];

        // Check if right channel exists (is not undefined) and average the channels
        // mono input stored in inputLeft
        if (inputRight) {
            for (let i = 0; i < inputLeft.length; i++) {
                inputLeft[i] = (inputLeft[i] + inputRight[i]) / 2;
            }
        }

        // send message to main thread containing the audio data
        this.port.postMessage({data: inputLeft});

        // If you want the audio processing to continue, return true.
        return true;
    }
}

registerProcessor('audio-accumulator', AudioAccumulator);
