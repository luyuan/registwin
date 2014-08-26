if (process.platform.toLowerCase() != 'win32') {
  throw new Error('RegistWin supports Windows Only.');
}
var package = require('../package.json');
var name = package.name;

var binding;
try {
  binding = require('../build/Release/' + name);
} catch (e) {
  try {
    binding = require('../build/Debug/' + name);
  } catch (e) { }
}

module.exports = binding;
