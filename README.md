## FFmpeg C Lib封装的前端WebAssembly（Wasm）库

### 项目描述
本项目是一个使用FFmpeg C Lib库封装的前端WebAssembly库，旨在为前端开发者提供H.264解码功能。通过使用FFmpeg C Lib，可以在Web浏览器环境中实现对H.264视频流的解码操作，以便在前端应用程序中进行视频处理和播放。

### 功能特性
* 提供H.264视频解码功能
* 可在前端Web浏览器中使用
* 基于FFmpeg C Lib实现，具有强大的解码能力和广泛的格式支持

### 安装和使用
以下是安装和使用该库的基本步骤：

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

### example

1. 运行脚本
```
bash run_example.sh
```

2. 浏览器打开 http://localhost:8080/test.html 

3. 选择项目下dump文件 即可播放

### 贡献和支持
欢迎对该项目进行贡献！如果你发现了问题或有改进建议，请在GitHub上提交issue或提出pull请求。

如果你对项目有任何疑问或需要支持，请联系我们或访问项目的GitHub页面。

### 许可证
该项目基于 MIT许可证，请查阅许可证文件获取更多信息。

### 鸣谢
感谢FFmpeg团队为我们提供强大的多媒体处理能力。此项目受益于FFmpeg C Lib的贡献和开源社区的支持。

### 参考项目

[https://github.com/xiangxud/webrtc_H265player](https://github.com/xiangxud/webrtc_H265player)

[https://github.com/ffmpegwasm/libav.wasm](https://github.com/ffmpegwasm/libav.wasm)

[https://github.com/oneam/h264bsd](https://github.com/oneam/h264bsd)

