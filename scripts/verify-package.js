const fs = require('fs');

const required = [
  'package.json',
  'README.md',
  'binding.gyp',
  'src/index.ts',
  'native/addon.cc',
  'native/mt4_manager_wrap.cc',
  'native/mt4_client.cc'
];

for (const file of required) {
  if (!fs.existsSync(file)) {
    throw new Error(`Missing required file: ${file}`);
  }
}

console.log('Package structure looks valid.');
