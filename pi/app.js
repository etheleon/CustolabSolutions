#!/usr/bin/env node

//var io = require('socket.io').listen(3636);

var clientio = require('socket.io-client');
var socket = clientio.connect('http://128.199.187.122:3636');

process.stdin.resume();
process.stdin.setEncoding('utf8');

process.stdin.on('data', function(data) {
  //process.stdout.write(data);
  var result = data.match(/Phase: (.*)?\nPhase Duration: (.*)?\nPhase Time left: (.*)?\nTotal Time left: (.*)?\n/);
  emitData = {"phase": result[1], "phaseDuration": result[2], "phaseTimeleft": result[3], "totaltime": result[4]};
  socket.emit('readout', emitData);
});
