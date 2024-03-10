const fs = require('fs');

const bin = fs.readFileSync(process.argv[2]);
const header = `!!! THIS IS A PICO FIRMWARE !!! PICO_FW_LEN=${bin.length}`;
const hfw = Buffer.from(header);
const outfw = Buffer.concat([hfw, Buffer.alloc(512 - hfw.length, 0), bin]);
fs.writeFileSync(process.argv[2] + ".fw", outfw);