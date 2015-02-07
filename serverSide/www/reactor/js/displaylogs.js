window.scrollTo(0,1);
$(document).ready(function() {
    var socket = io('http://altimit.mobi:9099');
    var indexes = ['TypeA', 'TypeB','TypeC'];

    //disconnect event
    socket.on('disconnect', function() {
        $("#Connectionstatus").text('Disconnected');
    });
    socket.on('connect', function() {
        $("#Connectionstatus").text('Connected');
    });

    var svg = d3.select("#PlotA")
    .append("svg")
    .attr("height",250)
    .attr("width",250)

var drawCircle = function(innersize,outersize,valueID,color) {
var arc = d3.svg.arc()
    .innerRadius(innersize)
    .outerRadius(outersize)
    .startAngle(0);

var arcTween = function(transition, newAngle) {
transition.attrTween("d", function(d) {
    var interpolate = d3.interpolate(d.endAngle, newAngle);
        return function(t) {
        d.endAngle = interpolate(t);
        return arc(d);
        };
        });
}


var background = svg.append("path")
    .datum({endAngle: Math.PI*2})
    .style("fill", "#ddd")
    .attr("d", arc)
    .attr("transform", function() { return "translate(" + "125" + ",120)"});

var foreground = svg.append("path")
.datum({endAngle: Math.PI})
.attr("d", arc(0))
.attr("transform", function() { return "translate(" + "125" + ",120)"})
.style("fill", color);

socket.on('new-data', function(data) {
    if(data.value.match(/____/)){
        var values = data.value.split("____");
        for(var i = 0; i <3; i++){
            $("#"+indexes[i]).text(indexes[i]+':    '+values[i]);                           //
        }
        foreground
        .transition()
        .duration(750)
        .call(arcTween, values[valueID]/20)
    }
});

$(window).resize(function() {
    //$("svg").attr( { "width":$(window).width(), "height":$(window).height()  });
    //foreground.attr("transform", function() { return "translate(" + $(window).width()/2 + ",250)"});
    //background.attr("transform", function() { return "translate(" + $(window).width()/2 + ",250)"});
});
}

drawCircle(90, 100,0, 'red');
drawCircle(70, 80,1, 'orange');
drawCircle(50, 60,2,'blue');
});
