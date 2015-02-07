window.scrollTo(0,1);
$(document).ready(function() {
    var socket = io('http://altimit.mobi:9099');
    //var indexes = ['TypeA', 'TypeB','TypeC'];

    //disconnect event
    socket.on('disconnect', function() {
        $("#Connectionstatus").text('Disconnected');
    });
    socket.on('connect', function() {
        $("#Connectionstatus").text('Connected');
    });

    socket.on('new-data', function(data) {
        console.log(data.value);
    });
});
