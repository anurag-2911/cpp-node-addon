const nodeaddon = require('./build/Debug/nodeaddon.node');
var res=nodeaddon.hello();
console.log(res);

module.exports = nodeaddon;