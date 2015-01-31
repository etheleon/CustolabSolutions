var connect = require('connect');
var serveStatic = require('serve-static');

var app = connect().use(serveStatic('/home/pi/tempfile')).listen(9099);
var socketio= require('socket.io')
var Tail = require('tail').Tail;

var io = socketio.listen(app);
console.log("Server listening on port 9099...");

var fileName 	= '/mnt/sensor/output.txt';
var tail = new Tail(fileName);

//this ties with the client side,
//where new-data is the event name
//sock.on('new-data', function(data) { do something in the client side js})
tail.on('line', function(data){
    io.sockets.emit('new-data', {channel: 'stdout', value: data});
});

//welcome message
io.on('connection', function(socket){
	var welcomeMessage = 'tail file ' +  fileName;
    	socket.emit('new-data',{channel: 'stdout', value: welcomeMessage});
});
