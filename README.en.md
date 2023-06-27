
## Frontend WebAssembly (Wasm) Library Wrapped with FFmpeg C Lib
### Project Description
This project is a frontend WebAssembly library that wraps the FFmpeg C Lib. Its purpose is to provide H.264 decoding functionality for frontend developers. By utilizing the FFmpeg C Lib, it enables H.264 video stream decoding within web browsers, allowing video processing and playback in frontend applications.

### Key Features
* Provides H.264 video decoding functionality
* Compatible with frontend web browsers
* Implemented using FFmpeg C Lib, offering powerful decoding capabilities and extensive format support

### Installation and Usage
Below are the basic steps to install and use the library:

```
git clone https://github.com/Fruneng/ffmpeg-h264-decoder-wasm.git
cd ffmpeg-h264-decoder-wasm
bash build.sh
```

```javascript
// Create the decoder and canvas
var decoder = new Worker('h264_worker.js');

var display = new Canvas(canvas, {'fullRangeColor': true});

console.log('Created decoder and canvas');

decoder.addEventListener('error', function(e) {
    console.log('Decoder error', e);
})

decoder.addEventListener('message', function(e) {
    var message = e.data;
    if (!message.hasOwnProperty('type')) return;

    switch(message.type) {
    case 'pictureReady':
        display.drawNextOutputPicture(
            message.width,
            message.height,
            message.croppingParams,
            new Uint8Array(message.data));
        ++pictureCount;
        break;
    case 'decoderReady':
        console.log('Decoder ready');
        break;
    }
});

```

```javascript
decoder.postMessage({
    'type' : 'input', 
    'data' : copy.buffer, 
}, [copy.buffer]);
```

### Contribution and Support
Contributions to this project are welcome! If you encounter any issues or have suggestions for improvement, please submit an issue or pull request on GitHub.

For questions or support regarding the project, feel free to contact us or visit the project's GitHub page.

### License
This project is licensed under the MIT License. Please refer to the license file for more information.

### Acknowledgements
Special thanks to the FFmpeg team for providing powerful multimedia processing capabilities. This project benefits from the contributions of the FFmpeg C Lib and the support of the open-source community.

### References

[https://github.com/xiangxud/webrtc_H265player](https://github.com/xiangxud/webrtc_H265player)

[https://github.com/ffmpegwasm/libav.wasm](https://github.com/ffmpegwasm/libav.wasm)

[https://github.com/oneam/h264bsd](https://github.com/oneam/h264bsd)