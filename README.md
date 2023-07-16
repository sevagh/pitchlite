# pitchlite

Simultaneous/parallel McLeod Pitch Method, designed for realtime WASM audio loop.

Resources:
* <https://www.toptal.com/webassembly/webassembly-rust-tutorial-web-audio>
* <https://www.toptal.com/webassembly/webassembly-rust-tutorial-web-audio#webassemblyrust-tutorial-getting-started>
* <https://developer.chrome.com/blog/audio-worklet-design-pattern/>

Simultaneous pitch tracking e.g. given an audio buffer of 4096 and subdivision of 512, return 9 pitches:
    overall pitch (4096)
    pitch 0 (0-512), pitch 1 (512-1024), pitch 2 (1024-1536), etc.
