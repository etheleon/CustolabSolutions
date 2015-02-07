var doc = $(document).ready();
$("#mainwindow").css({"padding-top":"55px"});
//Init
var 	width = $(document).width() - 10, 
	height = $(document).height() - 10 - 55;

//drag behaviour
var drag = d3.behavior.drag()
    .on("drag", dragmove)


//Append SVG -> CIRCLE
var svgContainer = d3.select("#mainwindow").append("svg")
                                    .attr("width", width - 20 )
                                    .attr("height",height - 20 )
    	    	    	    	    .append("circle")
    	    	    	    	    .attr("id", "bigreddot")
    	    	    	    	    .attr("cx", center(width))
    	    	    	    	    .attr("cy", center(height))
    	    	    	    	    .attr("r", 50)
    	    	    	    	    .style("fill", "red")
				    .call(drag)

//fxn for mouse drag behaviour
function dragmove(d) {
  d3.select(this)
      .attr("cx", d3.event.x)
      .attr("cy", d3.event.y)
}
function center (type) { 
	return Math.ceil(type/2);
}
$("circle").on('mouseup touchend', function() { 
	var randomNum = Math.random()
	if(randomNum > 0.6) { 
	alert("Stop Touching me!")
	}
})

//To deal with resizing
$(window).resize(function() {
  $("svg").attr( { "width":$(window).width() - 30, "height":$(window).height()-50-55  });
  $("#bigreddot").attr( {"cx":center($(window).width()-30), "cy":center($(window).height()-20)} );
});
