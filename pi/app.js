#!/usr/bin/env node

//var io = require('socket.io').listen(3636);

var clientio = require('socket.io-client');
var socket = clientio.connect('http://128.199.187.122:3636');

process.stdin.resume();
process.stdin.setEncoding('utf8');

process.stdin.on('data', function(data) {
  process.stdout.write(data);
  socket.emit('readout', data);
});
