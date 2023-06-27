function H264Decoder(module, channelName) {
  this.module = module;

  var _this = this;
  var frameCallback = module.addFunction(function (addr_y, addr_u, addr_v, stride_y, stride_u, stride_v, width, height) {
    console.log("[%d]In video callback, size = %d * %d, pts = %d", ++videoSize, width, height, pts)
    let out_y = HEAPU8.subarray(addr_y, addr_y + stride_y * height)
    let out_u = HEAPU8.subarray(addr_u, addr_u + (stride_u * height) / 2)
    let out_v = HEAPU8.subarray(addr_v, addr_v + (stride_v * height) / 2)
    let buf_y = new Uint8Array(out_y)
    let buf_u = new Uint8Array(out_u)
    let buf_v = new Uint8Array(out_v)
    let data = new Uint8Array(buf_y.length + buf_u.length + buf_v.length)
    data.set(buf_y, 0)
    data.set(buf_u, buf_y.length)
    data.set(buf_v, buf_y.length + buf_u.length)
    var obj = {
        data: data,
        width,
        height
    }
    var objData = {
        s: pts,
        d: obj
    };
    _this.postMessage(objData, [objData.d.data.buffer]);
  },'func');

  this._ctx = module._decoder_context_new(frameCallback);

  addEventListener(channelName, onMessage);
};

H264Decoder.prototype.onMessage = function(e) {
  var message = e.data;
  switch(message.type) {
  case 'input' :
    this.decode(message.data)
    break;
  }
}


H264Decoder.prototype.decode = function(data) {
  var module = this.module

  if (data instanceof ArrayBuffer) {
    data = new Uint8Array(data)
  }

  var pInput = module._malloc(data.byteLength);
  module.HEAPU8.set(data, pInput);

  module._decode(this._ctx, pInput, data.byteLength)
  module._free(pInput);
}

H264bsdDecoder.prototype.close = function() {
  var module = this.module
  module._decoder_context_free(this._ctx)
};

