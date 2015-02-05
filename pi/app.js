#!/usr/bin/env node

var io = require('socket.io').listen(3636);
process.stdin.resume();
process.stdin.setEncoding('utf8');

process.stdin.on('data', function(data) {
  process.stdout.write(data);
  io.emit('readout', data);
});
