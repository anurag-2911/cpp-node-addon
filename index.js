const nodeaddon = require('./build/Debug/nodeaddon.node');
var res=nodeaddon.hello();
var key= nodeaddon.getHKCUkey('Software\\Microfocus\\zapp','hashKey');
console.log('key is >>' + key);
deobfuscate(key, 'base64', 'utf8');
// console.log(res);

function deobfuscate(data, inputEncoding, outputEncoding){
    console.log('about to deobfuscate');

    var buffer = nodeaddon.obfuscate(new Buffer(data, inputEncoding));

    console.log('buffer is ' + buffer);
    
    var result = buffer.toString(outputEncoding);

    console.log('result is ' + result);


    
}


module.exports = nodeaddon;