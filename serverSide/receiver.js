#!/usr/bin/env node

var io = require('socket.io').listen(3636);

// Add a connect listener
io.sockets.on('connect',function(socket) {
    console.log('Client has connected to the server!');
    //once connected listen for event
    socket.on('readout',function(data) {
        io.emit('log', data)
        console.log(data.phase, data.phaseDuration, data.phaseTimeleft, data.totaltime);
    });
});
