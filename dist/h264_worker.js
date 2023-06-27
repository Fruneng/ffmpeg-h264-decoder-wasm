var decoder = null;

function onMessage(e) {
  var message = e.data;
  switch(message.type) {
  case 'input' :
      decoder.decode(message.data);
      break;
  }
}

addEventListener('message', onMessage);
importScripts('h264_decoder.js', 'decoder.js')

Module().then(module => {
  decoder = new H264Decoder(module, postMessage);
 
  postMessage({'type': 'decoderReady'});
})