function H264Decoder(module, postFunc) {
  this.module = module;

  var _this = this;
  var frameCallback = module.addFunction(function (addr_y, addr_u, addr_v, stride_y, stride_u, stride_v, width, height, pts) {
    let out_y = module.HEAPU8.subarray(addr_y, addr_y + stride_y * height)
    let out_u = module.HEAPU8.subarray(addr_u, addr_u + (stride_u * height) / 2)
    let out_v = module.HEAPU8.subarray(addr_v, addr_v + (stride_v * height) / 2)
    let buf_y = new Uint8Array(out_y)
    let buf_u = new Uint8Array(out_u)
    let buf_v = new Uint8Array(out_v)
    let data = new Uint8Array(buf_y.length + buf_u.length + buf_v.length)
    data.set(buf_y, 0)
    data.set(buf_u, buf_y.length)
    data.set(buf_v, buf_y.length + buf_u.length)
    var objData = {
        type: 'pictureReady',
        data: data,
        width: width,
        height, height,
        croppingParams: null,
    };
    postFunc(objData, [objData.data.buffer]);
  },'viiiiiiiii');

  this._ctx = module._decoder_context_new(frameCallback);
};

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

H264Decoder.prototype.close = function() {
  var module = this.module
  module._decoder_context_free(this._ctx)
};
