const worklet_chunk_size = 128;

let wasmModule;
let ptr;
let ptrPitches;
let stream;
let node;
let isStopped = false;

document.getElementById('start').addEventListener('click', async function() {
    isStopped = false;

    // Request access to the microphone
    stream = await navigator.mediaDevices.getUserMedia({ audio: true });

    // Create the audio context after obtaining the stream
    const audioContext = new AudioContext();
    console.log("Sample rate:", audioContext.sampleRate);

    let n_pitches;

    pitchlite().then((module) => {
        wasmModule = module;
        n_pitches = module._pitchliteInit(
            1024,
            512,
            audioContext.sampleRate, // use the actual sample rate of the audio context
            true, // use yin
            80, // mpm low pitch cutoff
        );

        // Create WASM views of the buffers, do it once and reuse
        ptr = wasmModule._malloc(worklet_chunk_size * Float32Array.BYTES_PER_ELEMENT);
        ptrPitches = wasmModule._malloc(n_pitches * Float32Array.BYTES_PER_ELEMENT);

        console.log("Created pitches buffer of size", n_pitches);
        console.log("WASM module initialized, buffers allocated");
    });

    audioContext.audioWorklet.addModule('audio-accumulator.js').then(() => {
      // Create an instance of your custom AudioWorkletNode
      node = new AudioWorkletNode(audioContext, 'audio-accumulator', {
        numberOfInputs: 1,
        numberOfOutputs: 1,
        outputChannelCount: [2],
      });

      // Connect the microphone stream to the processor
      const source = audioContext.createMediaStreamSource(stream);
      source.connect(node);

      // Connect to the destination
      node.connect(audioContext.destination);

      // In the onmessage event handler of your AudioWorkletNode.port
      // append received data to the ring buffer
      node.port.onmessage = function(event) {
        // Check if the "stop" button has been clicked
        if (isStopped) {
          return;
        }
        // event.data contains 128 samples of audio data from
        // the microphone through the AudioWorkletProcessor

        wasmModule.HEAPF32.set(event.data.data, ptr / Float32Array.BYTES_PER_ELEMENT);

        // Call the WASM function
        let retval = wasmModule._pitchlitePitches(ptr, worklet_chunk_size, ptrPitches);

        // copy the results back into a JS array
        // if retval is true, the pitch was calculated
        if (retval) {
          let wasmArrayPitches = new Float32Array(wasmModule.HEAPF32.buffer, ptrPitches, n_pitches);
          // Do something with the pitch
          console.log("pitches calculated!", wasmArrayPitches)
        }
      };
    });
});

document.getElementById('stop').addEventListener('click', async function() {
  console.log("Stopping and disconnecting and cleaning up")
  isStopped = true;

  // disconnect the audio worklet node
  node.disconnect();

  // stop tracks
  stream.getTracks().forEach(function(track) {
    console.log('Stopping stream');
    // Here you can free the allocated memory
    track.stop();
  });

  // cleanup
  wasmModule._free(ptrPitches);
  wasmModule._free(ptr);
});
