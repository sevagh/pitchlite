const sampleRate = 48000;
const audioContext = new AudioContext({ sampleRate: sampleRate });
const worklet_chunk_size = 128;

let node;
audioContext.audioWorklet.addModule('audio-accumulator.js').then(() => {
  // Create an instance of your custom AudioWorkletNode
  node = new AudioWorkletNode(audioContext, 'audio-accumulator', {
    numberOfInputs: 1,
    numberOfOutputs: 1,
    outputChannelCount: [2],
  });
  // Connect to the destination
  node.connect(audioContext.destination);
});

let wasmModule;
let n_pitches;
pitchlite().then((module) => {
  wasmModule = module;
  n_pitches = module._pitchliteInit(4096, 512, audioContext.sampleRate);
  console.log("WASM module initialized");
});

document.getElementById('start').addEventListener('click', async function() {
    // Request access to the microphone
    const stream = await navigator.mediaDevices.getUserMedia({ audio: true });

    // Connect the microphone stream to the processor
    const source = audioContext.createMediaStreamSource(stream);
    source.connect(node);

    // Connect the processor to the audio context's destination
    // no need if no outputs
    node.connect(audioContext.destination);

    // Create WASM views of the buffers, do it once and reuse
    let ptr = wasmModule._malloc(worklet_chunk_size * Float32Array.BYTES_PER_ELEMENT);
    let ptrPitches = wasmModule._malloc(n_pitches * Float32Array.BYTES_PER_ELEMENT);

    console.log("Created pitches buffer of size", n_pitches);

    // In the onmessage event handler of your AudioWorkletNode.port
    // append received data to the ring buffer
    node.port.onmessage = function(event) {
      // event.data contains 128 samples of audio data from
      // the microphone through the AudioWorkletProcessor

      wasmModule.HEAPF32.set(event.data, ptr / event.data.BYTES_PER_ELEMENT);

      let ptrPitches = wasmModule._malloc(n_pitches * Float32Array.BYTES_PER_ELEMENT);

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

    // cleanup
    wasmModule._free(ptrPitches);
    wasmModule._free(ptr);
});
