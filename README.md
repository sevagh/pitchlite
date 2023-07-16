# pitchlite

Simultaneous/parallel McLeod Pitch Method, designed for realtime WASM audio loop.

Simultaneous pitch tracking e.g. given an audio buffer of 4096 and subdivision of 512, return 9 pitches:
    pitch 0-7: (0-512), (512-1024), (1024-1536), etc.
    pitch 8: overall pitch (4096)

Code copied from <https://github.com/sevagh/pitch-detection>, with an improvement: autocorrelation now entirely uses real FFTs (and uses KissFFT instead of FFTS).

## Usage

The `/web` directory shows an example app that uses the AudioWorklet API to send 128 samples at a time into the pitchlite module.

As always, clone with git submodules for included KissFFT:
```
$ git clone --recurse-submodules https://github.com/sevagh/pitchlite
```

* pitchlite.js, pitchlite.wasm: compiled WASM module:
```
sevagh@pop-os:~/repos/pitchlite$ rm -rf build-wasm && mkdir -p build-wasm && cd build-wasm && emcmake cmake .. && make

// this will build pitchlite.js, pitchlite.wasm and copy them to ./web
```

Arguments: see index.js for how to initialize and use the pitchlite module

* audio-accumulator.js: audio worklet processor that just sends 128 samples to index.js
* index.js: initialize wasm module, create "ring buffer" pointer of size 4096
    * receive 128 samples at a time, copy into ring buffer, advanced nAccumulated pointer
    * once 4096/128 have been accumulated, compute pitch of entire ring buffer/4096 samples
        * print pitch
        * reset accumulator
        * reset ringbuffer

## Run it yourself

Point any local web server to the `web` directory, e.g. with Python:
```
sevagh@pop-os:~/repos/pitchlite$ python -m http.server -d ./web/
Serving HTTP on 0.0.0.0 port 8000 (http://0.0.0.0:8000/) ...
127.0.0.1 - - [16/Jul/2023 16:38:35] "GET / HTTP/1.1" 200 -
...
```

Visit http://0.0.0.0:8000/ and press the buttons. Open console.log to see what's happening.

## Improvement ideas

* Switching from float to double may improve Mpm or Yin pitch values (at the cost of memory and potentially computation cost)
* The web code is low quality and only an example of how to use the C++ WASM module
