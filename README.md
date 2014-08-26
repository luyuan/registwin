## RegistWin - A Windows Registry Extension for Node.js

 * Create, open, iterate and watch the key of Windows Registry
 * Read and write the value of the key

This extension is only supported on Windows.

### Usage
```js
var RegistWin = require('registwin').RegistWin;
var reg = new RegistWin(RegistWin.HKEY_CLASSES_ROOT);
reg.openKey('*', RegistWin.KEY_ALL_ACCESS);
var quickTip = reg.readValue('QuickTip');
console.log(quickTip);  // "prop:System.ItemTypeText;System.Size;System.DateModified
reg.iterateKeys(function (index, name) {
  console.log(name)  // "OpenWithList", "shell", "shellex"
});
```
