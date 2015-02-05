#!/usr/bin/env node

var io = require('socket.io-client');
var socket = io.connect('http://127.0.0.1:3636');

// Add a connect listener
socket.on('connect',function() {
    console.log('Client has connected to the server!');
});
// Add a connect listener
socket.on('readout',function(data) {
    console.log(data);
});
// Add a disconnect listener
socket.on('disconnect',function() {
    console.log('The client has disconnected!');
});

// Sends a message to the server via sockets
//function sendMessageToServer(message) {
    //socket.send(message);
//};
