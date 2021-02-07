const testAddon = require('./build/Debug/testaddon.node');
var res=testAddon.hello();
console.log(res);

module.exports = testAddon;