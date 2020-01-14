// Global var for all the csv data
var playerData, corrMatrix, lineChartDataA, lineChartDataB, 
    scatterCPaul, scatterJKidd, scatterSNash, scatterSCurry;

/**
 * Render and update the bar chart based on the selection of the data type in the drop-down box
 *
 * @param selectedDimension a string specifying which dimension to render in the bar chart
 */
function updateBarChart() {

    var svgBounds = d3.select("#barChart").node().getBoundingClientRect(),
        margin = {top: 40, right: 20, bottom: 60, left: 30};

    // Create the x and y scales; make
    // sure to leave room for the axes
    var svg = d3.select("#barChart");

    // here we use an ordinal scale with scaleBand to position and size the bars in x direction
    var xScale = d3.scaleBand()
            .domain(playerData.map(function (d) {//.slice(0).reverse()
                return d.Season;
            }))
            .range([margin.left, svgBounds.width])
            .padding(.1);
    
    var maxY = d3.max(playerData, function (d) {return 1.3 * d['AST%'] / d['USG%'];}),
    yScale = d3.scaleLinear()
            .domain([0, maxY])
            .range([svgBounds.height - margin.bottom, 0]);

    // we identify the button and add the function to be connected dynamically
    document.getElementById("maxA/U").innerHTML = maxY;

    // Create colorScale
    var colorScale = d3.scaleSequential()//d3.scaleLinear()
            .domain([0.5, maxY])
            .interpolator(d3.interpolateBlues);
            //.range(["lightsteelblue", "steelblue"]);

    // Create the axes (hint: use #xAxis and #yAxis)
    svg.select("#xAxis")
            .attr("transform", "translate(0," + (svgBounds.height - margin.bottom) + ")")
            .transition()
            .duration(1000)
            .call(d3.axisBottom(xScale))
            .selectAll("text")
                // align it to the right
                .style("text-anchor", "end")
                .attr("dx", "-.8em")
                // dy is a shift along the y axis
                .attr("dy", "-.6em")
                .attr("transform", "rotate(-70)")
                // center it
                .attr("alignment-baseline", "middle")

    svg.select("#yAxis")
            .attr("transform", "translate(" + margin.left + ",0)")
            .transition()
            .duration(1000)
            .call(d3.axisLeft(yScale));

    // Create the bars (hint: use #bars)
    var bars = svg.select("#bars").selectAll("rect")
            .data(playerData, function (d) {
                return d.Season;
            });

    //---------------- Exit and Exit Animations ------------------------

    bars.exit()
        .attr("opacity", 1)
        .transition()
        .duration(600)
        .attr("opacity", 0)
        .remove();

    //---------------- Enter and Enter Animations ------------------------


    bars = bars.enter().append("rect").merge(bars);
    bars.attr("x", function (d) {
                return xScale(d.Season);
            })
            .attr("y", function (d) {
                return yScale(d['AST%'] / d['USG%']);
            })
            .attr("width", xScale.bandwidth())
            .attr("height", function (d) {
                return svgBounds.height - margin.bottom - yScale(d['AST%'] / d['USG%']);
            })
            .style("fill", function (d) {
                return colorScale(d['AST%'] / d['USG%']);
            })/*
            .on("mouseenter", function (d, actual, i) {

                var selected = d3.select(this);

                selected
                    .transition()
                    .duration(300)
                    .attr("opacity", 0.7)
                    .attr("x", function (d) {
                        return xScale(d.Season) - 5;
                    })
                    .attr("width", xScale.bandwidth() + 10);
                
                const y = yScale(actual.value);
                console.log("yScale(actual.value)=" + y);

                line = barsEnter.append('line')
                    .attr('id', 'limit')
                    .attr('x1', margin.left)
                    .attr('y1', y)
                    .attr('x2', svgBounds.width)
                    .attr('y2', y);

                // Display the selected data in the chart
                selected.append("text")
                    .attr("class", "display")
                    .attr("x", function (d) {
                        return xScale(d.Season) + xScale.bandwidth() / 2;
                    })
                    .attr("text-anchor", "start")
                    .attr("y", function (d) {
                        return yScale(d[selectedDimension]);
                    })
                    .attr("text-anchor", "middle")
                    .attr("transition", "rotate(-90)") // transform
                    .text(function (d) {
                        return "";//d[selectedDimension];
                    });

            })
            .on("mouseleave", function (d) {

                var selected = d3.select(this);

                selected
                    .transition()
                    .duration(300)
                    .attr("opacity", 1)
                    .attr("x", function (d) {
                        return xScale(d.YESeasonAR);
                    })
                    .attr("width", xScale.bandwidth());

                svg.selectAll(".display").remove()
            })*/;


    //--------- Update Animations when change the drop down list----------

    bars
        .attr("opacity", 0)
        .transition()
        .duration(1000)
        .attr("opacity", 1)
        .attr("x", function (d) { // Including update the x axes!!!
            return xScale(d.Season);
        })
        .attr("y", function (d) {
            return yScale(d['AST%'] / d['USG%']);
        })
        .attr("height", function (d) {
            return svgBounds.height - margin.bottom - yScale(d['AST%'] / d['USG%']);
        })
        .style("fill", function (d) {
            return colorScale(d['AST%'] / d['USG%']);
        });


    // Implement how the bars respond to click events
    // Color the selected bar to indicate is has been selected.
    // Make sure only the selected bar has this new color.
    bars.on("click", function (d, i) {
        svg.selectAll("rect")
            .style("fill", function (d) {
                return colorScale(d['AST%'] / d['USG%']);
            });

        d3.select(this)
            .transition()
            .duration(300)
            .attr("opacity", 1)
            .style("fill", "red");// ForestGreen // PeachPuff

        document.getElementById("currentA/U").innerHTML = 
            "当前选中赛季的“助攻率为" + d['AST%'] + "%，回合占有率为" + d['USG%'] + "%，之比为" + d['AST%'] / d['USG%'] + "！";
    });

}

/**
 *  Check the drop-down box for the currently selected data type and update the bar chart accordingly.
 *
 */
function chooseData() {

    //Changed the selected data when a user selects a different
    // menu item from the drop down.
    var active = document.getElementById("topPG").value;
    /*
    switch (active) {
        case "matches":
            active = "MATCHES";
            break;
        case "attendance":
            // fall through
        case "teams":
        case "goals":
    }*/
    d3.csv("data/ast%-usg%/" + active + ".csv", function (error, csv) {

        playerData = csv;
        updateBarChart();

    })

    document.getElementById("currentA/U").innerHTML = "点击条带显示当前数据！";

}

function choosePlayer() {
    var active = document.getElementById("shooting_player").value;
    d3.csv("data/shooting-margin-value-5pts/" + active + ".csv",
    function (error, csv) {
        lineChartDataA = csv;
    });
}

function chooseStat() {
    var active = document.getElementById("stat").value;
    updateLineChart(active);
}

function chooseYear() {
    //Changed the selected data when a user selects a different
    // menu item from the drop down.
    var active = document.getElementById("year").value;
    d3.csv("data/pg_per-100-poss/corrMatrix/corrMatrix_reshape_" + active + ".csv", function (error, csv) {
        corrMatrix = csv;
        updateHeatmap();
    });
}

/**
 * Usage HEATMAP
 * @param {*} _param_ 
 */
function updateHeatmap() {

    var svgBounds = d3.select("#heatmap").node().getBoundingClientRect(),
        margin = {top: 20, right: 20, bottom: 60, left: 100};

    var svg = d3.select("#heatmap");

    //--------------- Scale ---------------

    var xScale = d3.scaleBand()
            .domain(d3.map(corrMatrix, function (d) {
                return d.x;
            }).keys())
            .range([margin.left, svgBounds.width - margin.right])
            .padding(.05);
    var yScale = d3.scaleBand()
            .domain(d3.map(corrMatrix, function (d) {
                return d.y;
            }).keys())
            .range([svgBounds.height - margin.bottom, margin.top])
            .padding(.05);

    //--------------- Axis ---------------

    svg.select("#row")
            .attr("transform", "translate(0," + (svgBounds.height - margin.bottom) + ")")
            .call(d3.axisBottom(xScale).tickSize(0))
            .select(".domain").remove();

    svg.select("#row").selectAll("text")
                .style("text-anchor", "end")
                //dx is a shift along y-axis
                .attr("dx", "-.3em")
                //dy is a shift along x-axis
                .attr("dy", ".0em")
                .attr("transform", "rotate(-70)")
                .attr("alignment-baseline", "middle");

    svg.select("#column")
            .style("text-anchor", "end")
            .attr("transform", "translate(" + margin.left + ",0)")
            .call(d3.axisLeft(yScale).tickSize(0))
            .select(".domain").remove();

    //--------------- Color scale ---------------

    var colorScale = d3.scaleSequential()
            .domain([-1.0,1.0]) // [1,varible.length] // [1,100]
            .interpolator(d3.interpolateInferno);

    //--------------- Tooltip ---------------

    var tooltip = d3.select("#tool-tip")
            .style("left","250px")
            .style("top","300px");

    //--------------- Mouse events ---------------

    var mouseover = function (d) {
        tooltip
            .style("opacity", 1)
        d3.select(this)
            .style("stroke", "black")
            .style("opacity", 1);
    }

    var mousemove = function (d) {
        tooltip
            .html("corr(" + d.x + "," + d.y + ")=" + d.value)
            .style("left", (d3.mouse(this)[0]+50) + "px")
            .style("top", (d3.mouse(this)[1]) + "px");
      }

    var mouseleave = function (d) {
        tooltip
            .style("opacity", 0);
        d3.select(this)
            .style("stroke", "none")
            .style("opacity", 0.8);
      }

    //--------------- Enter and Enter Animations ---------------

    var gridEnter = svg.select("#grid")
            .selectAll("rect")
            .data(corrMatrix, function (d) {
                return d.x + ":" + d.y;
            })
            .enter()
            .append("rect");
    gridEnter
            .attr("x", function (d) {
                return xScale(d.x) + xScale.bandwidth() * (1 - Math.abs(d.value)) / 2;
            })
            .attr("y", function (d) {
                return yScale(d.y) + yScale.bandwidth() * (1 - Math.abs(d.value)) / 2;
            })
            // rx and ry are the radii of the ellipse
            .attr("rx", 4)
            .attr("ry", 4)
            .attr("width", function (d) {
                return xScale.bandwidth() * Math.abs(d.value);
            })
            .attr("height", function (d) {
                return yScale.bandwidth() * Math.abs(d.value);
            })
            .style("fill", function (d) {return colorScale(d.value);})
            .style("stroke-width", 4)
            .style("stroke", "none")
            .style("opacity", 1)
            .on("mouseover", mouseover)
            .on("mousemove", mousemove)
            .on("mouseleave", mouseleave);


    // Add title to graph
    svg.select("#hmTitle")
            .append("text")
            .attr("x", 0)
            .attr("y", -50)
            .attr("text-anchor", "left")
            .style("font-size", "22px")
            .text("A d3.js heatmap");

    // Add subtitle to graph
    svg.select("#hmTitle")
            .append("text")
            .attr("x", 0)
            .attr("y", -20)
            .attr("text-anchor", "left")
            .style("font-size", "14px")
            .style("fill", "grey")
            .style("max-width", 400)
            .text("A short description of the take-away message of this chart.");

    //--------- Update Animations when change the drop down list----------

    svg.select("#grid")
            .selectAll("rect")
            .data(corrMatrix, function (d) {
                return d.x + ":" + d.y;
            })
            .transition()
            .duration(500)
            .attr("x", function (d) {
                return xScale(d.x) + xScale.bandwidth() * (1 - Math.abs(d.value)) / 2;
            })
            .attr("y", function (d) {
                return yScale(d.y) + yScale.bandwidth() * (1 - Math.abs(d.value)) / 2;
            })
            .attr("width", function (d) {
                return xScale.bandwidth() * Math.abs(d.value);
            })
            .attr("height", function (d) {
                return yScale.bandwidth() * Math.abs(d.value);
            })
            .style("fill", function (d) {return colorScale(d.value);});

}

/**
 * For slider hint text
 * @param {*} props 
 */
function drawText (props) {
    var selector = props.selector;
    var height = props.height;
    var xOffset = props.xOffset;
    var yOffset = props.yOffset;
    var text = props.text;
    d3.select(selector).append('g').attr('transform', 'translate(' + xOffset + ',' + yOffset + ')').append('text').style('fill', '#666').style('fill-opacity', 1).style('pointer-events', 'none').style('stroke', 'none').style('font-size', 10).text(text);
}

/**
 * For slider
 * @param {*} props 
 */
function drawSliderControl (props) {
    var selector = props.selector;
    var padding = props.padding;
    var defaultMarkOpacity = props.defaultMarkOpacity;
    var defaultLinkOpacity = props.defaultLinkOpacity;
    var defaultLabelOpacity = props.defaultLabelOpacity;

    d3.select(selector).select('input').attr('type', 'range').attr("float", "right").attr('min', 0.0).attr('max', 1.0).attr('value', 0.0).attr('step', 0.0001).style('top', '375px').style('right', '0px').style('height', '36px').style('width', '300px').attr('id', 'slider');

    d3.select('#slider').on('input', function () {
        update(+this.value);
    });

    /**
     * Inner function
     * @param {*} sliderValue 
     */
    function update (sliderValue) {

        // fade heat bars below the threshold
        d3.selectAll('#grid > rect').style('fill-opacity', function (d) {
            // first style the label associated with the mark
            /*
            d3.select('#node' + d.id).selectAll('.label').style('fill-opacity', function () {
                if (d.maxLinkWeight < sliderValue) {
                    return 0.1;
                }
                return defaultLabelOpacity;
            });
            */

            // then style the mark itself
            if (Math.abs(d.value) < sliderValue) {
                return Math.max(0.1, defaultMarkOpacity - 0.6);
            }
            return defaultMarkOpacity;
        });

        // if there is a pictogram table on the page
        /*
        if (d3.select('.pictogramTable').nodes().length > 0) {
            // fade table text for rows below the threshold
            d3.select('.pictogramTable').selectAll('tr').style('color', function (d) {
                // first style the label associated with the mark
                // console.log('d from span selection', d);
                if (d.weight < sliderValue) {
                    return '#CCC';
                }
            return 'black';
            });
        }
        */
    }
}

/**
 * 
 * @param {*} selectedCol 
 */
function updateLineChart (selectedCol) {

    var svgBounds = d3.select("#linePlot").node().getBoundingClientRect(),
    margin = {top: 20, right: 0, bottom: 100, left: 30};

    var svg = d3.select("#linePlot");

    // Add X axis --> it is a date format
    var xScale = d3.scaleBand()
            .domain(d3.map(lineChartDataA, function (d) {
            return d.Season;
        }).keys())
        .range([margin.left, svgBounds.width - margin.right])
        .padding(.1);

    svg.select("#year")
        .attr("transform", "translate(0," + ((svgBounds.height - margin.bottom - margin.top) / 2 + margin.top) + ")")
        .call(d3.axisBottom(xScale));

    svg.select("#year").selectAll("text")
        .style("text-anchor", "end")
        .attr("dx", "-.8em")
        // dy is a shift along the y axis
        .attr("dy", "-.6em")
        .attr("transform", "rotate(-70)")
        // center it
        .attr("alignment-baseline", "middle");

    // Add Y axis
    var yMaxA = d3.max(lineChartDataA, function (d) {return +d[selectedCol];}),
        yMinA = d3.min(lineChartDataA, function (d) {return +d[selectedCol];}),
        yMaxB = d3.max(lineChartDataB, function (d) {return +d[selectedCol];}),
        yMinB = d3.min(lineChartDataB, function (d) {return +d[selectedCol];}),
        yMax = Math.max(yMaxA, yMaxB),
        yMin = Math.min(yMinA, yMinB);

    var yScale = d3.scaleLinear()
        .domain([yMin, yMax])
        .range([svgBounds.height - margin.bottom, margin.top]);

    svg.select("#pointValue")
        .attr("transform","translate(" + margin.left + ",0)")
        .transition()
        .duration(1000)
        .call(d3.axisLeft(yScale));

    svg.select("#PointValue").selectAll("text")
        .style("text-anchor", "end")
        .attr("dx", "-.8em")
        // dy is a shift along the y axis
        .attr("dy", "-.6em")
        // center it
        .attr("alignment-baseline", "middle");


    //-------------- Add multuple lines --------------

    // Exit old lines!
    svg.selectAll(".linepath").remove();

    // Add the line A
    svg.datum(lineChartDataA)
        .append("path")
        .attr("class", "linepath")
            .attr("fill", "none")
            .attr("stroke", "#136bddb9")
            .attr("stroke-width", 1.5)
            .attr("d", d3.line()
                .x(function (d) {return xScale(d.Season);})
                .y(function (d) {return yScale(+d[selectedCol]);})
            );

    // Exit old dots!
    var dotsA = svg.selectAll(".dotA");
    dotsA
        .exit()
        .remove();

    // Add new dots A!
    dotsA
        .data(lineChartDataA)
        .enter()
        .append("circle")
        .merge(dotsA)
            .attr("class", "dotA")
            .attr("fill", "#136bddb9")
            .attr("stroke", "#136bddb9")
            .attr("opacity", 0.5)
            .attr("cx", function (d) {return xScale(d.Season);})
            .attr("cy", function (d) {return yScale(+d[selectedCol]);})
            .attr("r", 5);


    // Then add data B line
    svg.datum(lineChartDataB)
        .append("path")
        .attr("class", "linepath")
                .attr("fill","none")
                .attr("stroke","rgba(226, 152, 13, 0.678)")
                .attr("stroke-dasharray", "10,10")
                .attr("stroke-width", 1.5)
                .attr("d", d3.line()
                        .x(function (d) {return xScale(d.Season);})
                        .y(function (d) {return yScale(+d[selectedCol]);})
                    );

    // Exit and add new dots!
    var dotsB = svg.selectAll(".dotB");
    dotsB
        .exit()
        .remove();

    // Add new dots A!
    dotsB
        .data(lineChartDataB)
        .enter()
        .append("circle")
        .merge(dotsB)
            .attr("class", "dotB")
            .attr("fill", "rgba(226, 152, 13, 0.678)")
            .attr("stroke", "rgba(226, 152, 13, 0.678)")
            .attr("opacity", 0.5)
            .attr("cx", function (d) {return xScale(d.Season);})
            .attr("cy", function (d) {return yScale(+d[selectedCol]);})
            .attr("r", 5);

}



function updateLineChart_2 (selectedCol) {
    
    var svgBounds = d3.select("#linePlot").node().getBoundingClientRect(),
    margin = {top: 20, right: 0, bottom: 100, left: 30};

    var svg = d3.select("#linePlot");

    // Add X axis --> it is a date format
    var xScale = d3.scaleBand()
            .domain(d3.map(lineChartDataA, function (d) {
            return d.Season;
        }).keys())
        .range([margin.left, svgBounds.width - margin.right])
        .padding(.1);


    // Add Y axis
    var yMaxA = d3.max(lineChartDataA, function (d) {return +d[selectedCol];}),
        yMinA = d3.min(lineChartDataA, function (d) {return +d[selectedCol];}),
        yMaxB = d3.max(lineChartDataB, function (d) {return +d[selectedCol];}),
        yMinB = d3.min(lineChartDataB, function (d) {return +d[selectedCol];}),
        yMax = Math.max(yMaxA, yMaxB),
        yMin = Math.min(yMinA, yMinB);

    var yScale = d3.scaleLinear()
        .domain([yMin, yMax])
        .range([svgBounds.height - margin.bottom, margin.top]);

    svg.select("#pointValue")
        .attr("transform","translate(" + margin.left + ",0)")
        .transition()
        .duration(1000)
        .call(d3.axisLeft(yScale));
/*
    svg.select("#PointValue").selectAll("text")
        .style("text-anchor", "end")
        .attr("dx", "-.8em")
        // dy is a shift along the y axis
        .attr("dy", "-.6em")
        // center it
        .attr("alignment-baseline", "middle");
*/

    //-------------- Add multuple lines --------------

    // Add the line A
    var pathA = svg
        .selectAll(".linepathA")
        .datum(lineChartDataA);

    // exit old line
    pathA
        //.selectAll(".linepathA")
        //.exit()
        .attr("opacity", 1)
        .transition()
        .duration(1000)
        .attr("opacity", 0)
        .remove();

}


function updateScatterPlot () {

    var svgBounds = d3.select("#scatterPlot").node().getBoundingClientRect(),
        margin = {top: 20, right: 250, bottom: 30, left: 40};

    // List of groups (here I have one group per column)
    var allGroup = ["Chris Paul", "Steve Nash", "Jason Kidd", "Stephen Curry"];

    var svg = d3.select("#scatterPlot");

    // A color scale: one color for each group
    var colorScale = d3.scaleOrdinal()
        .domain(allGroup)
        .range(d3.schemeSet1);

    // Add X scale and axis
    
    /*maxArr = [];
    scatterData.forEach(function (row) {
        maxArr.push(d3.max(row.values, function (d) {
            return d.usg_ptg;
        }));
    });
    var maxX = d3.max(maxArr, function (d) {
        return d;
    });
    */
    var xScale = d3.scaleLinear()
            .domain([0,40.0]) // mins
            .range([margin.left, svgBounds.width - margin.right]);

    svg.select("#xAxis")
            .attr("transform", "translate(0," + (svgBounds.height - margin.bottom) + ")")
            .call(d3.axisBottom(xScale));

    // Add X axis label:
    svg.select("#xAxis")
        .append("text")
            .attr("text-anchor", "end")
            .attr("x", svgBounds.width / 2)
            .attr("y", svgBounds.height)
            .text("USG%");

    // Add Y scale and axis
    /*
    maxArr = [];
    scatterData.forEach(function (row) {
        maxArr.push(d3.max(row.value, function (d) {
            return d.ast_ptg;
        }));
    });
    var maxY = d3.max(maxArr, function (d) {
        return d;
    });
    */
    var yScale = d3.scaleLinear()
            .domain([0,60.0]) // scores
            .range([svgBounds.height - margin.bottom, margin.top]);

    svg.select("#yAxis")
            .attr("transform", "translate(" + margin.left + ",0)")
            .call(d3.axisLeft(yScale));

    // Add Y axis label:
    svg.select("#yAxis")
        .append("text")
            .attr("x", 0)
            .attr("y", margin.top - 5)
            .text("AST%")
            .attr("text-anchor", "start")


    // Scale the circle (bubble size, the radius)
    var rScale = d3.scaleLinear()
            .domain([0, 30]) // scores by 'TOV%'
            .range([10, 1]);


    // ---------------------------//
    //      TOOLTIP               //
    // ---------------------------//

    // -1- Create a tooltip div that is hidden by default:
    var tooltip = d3.select("#scatter_div")
            .append("div")
            .style("opacity", 0)
            .attr("class", "tooltip")
            .style("background-color", "black")
            .style("border-radius", "5px")
            .style("padding", "10px")
            .style("color", "white");

    // -2- Create 3 functions to show / update (when mouse move but stay on same circle) / hide the tooltip
    var showTooltip = function (d) {
        tooltip
            .transition()
            .duration(200);
        tooltip
            .style("opacity", 1)
            .html("Season " + d['Season'] + ", Ast% / Usg% : " + d['AST%'] / d['USG%'])
            .style("left", (d3.mouse(this)[0]+30) + "px")
            .style("top", (d3.mouse(this)[1]+30) + "px");
    }

    var moveTooltip = function (d) {
        tooltip
            .style("left", (d3.mouse(this)[0]+30) + "px")
            .style("top", (d3.mouse(this)[1]+30) + "px");
        }

    var hideTooltip = function (d) {
        tooltip
            .transition()
            .duration(200)
            .style("opacity", 0);
        };

  //--------------------------------//
  //       Add dots, no lines       //
  //--------------------------------//
//d3.selectAll(".bubbles").style("opacity", 1);
    svg.select("#dot-1")
            .selectAll("circle")
            .data(scatterCPaul)
            .enter()
            .append("circle")
                .attr("class", function(d) { return "bubbles cpaul";})
                .attr("cx", function (d) {return xScale(+d['USG%']);})
                .attr("cy", function (d) {return yScale(+d['AST%']);})
                .attr("r", function (d) {return rScale(+d['TOV%']);})
                .attr("fill", function(d){return colorScale(allGroup[0]);})//"#06a4ff"
                .on("mouseover", showTooltip)
                .on("mousemove", moveTooltip)
                .on("mouseleave", hideTooltip);

    svg.select("#dot-2")
            .selectAll("circle")
            .data(scatterSNash)
            .enter()
            .append("circle")
                .attr("class", function(d) {return "bubbles snash";})
                .attr("cx", function (d) {return xScale(+d['USG%']);})
                .attr("cy", function (d) {return yScale(+d['AST%']);})
                .attr("r", function (d) {return rScale(+d['TOV%']);})
                .attr("fill", function(d){return colorScale(allGroup[1]);})//"#ff6106"
                .on("mouseover", showTooltip)
                .on("mousemove", moveTooltip)
                .on("mouseleave", hideTooltip);

    svg.select("#dot-3")
            .selectAll("circle")
            .data(scatterJKidd)
            .enter()
            .append("circle")
                .attr("class", function(d) {return "bubbles jkidd";})
                .attr("cx", function (d) {return xScale(+d['USG%']);})
                .attr("cy", function (d) {return yScale(+d['AST%']);})
                .attr("r", function (d) {return rScale(+d['TOV%']);})
                .attr("fill", function(d){return colorScale(allGroup[2]);})//"#09b33ccb"
                .on("mouseover", showTooltip)
                .on("mousemove", moveTooltip)
                .on("mouseleave", hideTooltip);
    
    svg.select("#dot-4")
            .selectAll("circle")
            .data(scatterSCurry)
            .enter()
            .append("circle")
                .attr("class", function(d) { return "bubbles scurry";})
                .attr("cx", function (d) {return xScale(+d['USG%']);})
                .attr("cy", function (d) {return yScale(+d['AST%']);})
                .attr("r", function (d) {return rScale(+d['TOV%']);})
                .attr("fill", function(d){return colorScale(allGroup[3]);})//"#eeff06cb"
                .on("mouseover", showTooltip)
                .on("mousemove", moveTooltip)
                .on("mouseleave", hideTooltip);


    // ---------------------------//
    //       HIGHLIGHT GROUP      //
    // ---------------------------//

    // What to do when one group is hovered
    var highlight = function (d) {
        // reduce opacity of all groups
        d3.selectAll(".bubbles").style("opacity", .05);
        // expect the one that is hovered
        d3.selectAll("." + d).style("opacity", 1);
    };

    // And when it is not hovered anymore
    var noHighlight = function (d) {
        d3.selectAll(".bubbles").style("opacity", 1);
    };


    // ---------------------------//
    //       LEGEND              //
    // ---------------------------//

    // Add one dot in the legend for each name.
    var size = 20;
    svg.select("#scatterLegend")
        .selectAll("#legendIcon")
        .data(["", "cpaul", "snash", "jkidd", "scurry"]) // ????????
        .enter()
        .append("circle")
            .attr("cx", 420)
            .attr("cy", function(d,i){ return 10 + i*(size+5);}) // 100 is where the first dot appears. 25 is the distance between dots
            .attr("r", 7)
            .style("fill", function(d, i){return colorScale(allGroup[i-1]);})
            .on("mouseover", highlight)
            .on("mouseleave", noHighlight);

    // Add labels beside legend dots
    svg.select("#scatterLegend")
        .selectAll("#legendLabel")
        .data(["", "cpaul", "snash", "jkidd", "scurry"])
        .enter()
        .append("text")
            .attr("x", 420 + size*.8)
            .attr("y", function(d,i){return i * (size + 5) + (size/2)}) // 100 is where the first dot appears. 25 is the distance between dots
            .style("fill", function(d, i){return colorScale(allGroup[i-1]);})
            .text(function(d, i){return allGroup[i-1];})
            .attr("text-anchor", "left")
            .style("alignment-baseline", "middle")
            .on("mouseover", highlight)
            .on("mouseleave", noHighlight);
}


/* DATA LOADING */

// This is where execution begins; everything
// above this is just function definitions
// (nothing actually happens)

// Load CSV file
d3.csv("data/ast%-usg%/cpaul.csv", function (error, csv) {

    // Store csv data in a global variable
    playerData = csv;

    //--------------- Add slider ---------------

    d3.select('#heat-map').selectAll('.sliderTextSVG').append('svg').attr('height', 100).attr('width', 450).attr('class', 'sliderTextSVG');

    // draw the help text for the slider
    drawText({
        selector: '.sliderTextSVG',
        text: 'Slide rightward (-->) to increase the correlation threshold!',
        xOffset: 0,
        yOffset: 0
    });

    // draw the slider control
    drawSliderControl({
        selector: 'div#slider-container',
        padding: '10px',
        defaultLinkOpacity: 0.4,
        defaultMarkOpacity: 1,
        defaultLabelOpacity: 1
    });


    // Draw the Bar chart for the first time

    updateBarChart();

});

d3.csv("data/pg_per-100-poss/corrMatrix/corrMatrix_reshape_14.csv", function (error, csv) {

    corrMatrix = csv;
    updateHeatmap();

});

//--------- Update Animations when change the drop down list----------

d3.csv("data/shooting-margin-value-5pts/cpaul.csv", function (error, csv) {
    /*csv.forEach(function (d) {
        // Convert numeric values to 'numbers'
        d['FG%'] = +d['FG%'];
        d['3P%'] = +d['3P%'];
        d['eF%'] = +d['eFG%'];
    })*/
    lineChartDataA = csv;
});

d3.csv("data/player_per-game/cpaul.csv", function (error, csv) {
    lineChartDataB = csv;
    updateLineChart('FG%'); // FG% 3P% eFG%
});



//--------- Add scatter ---------

d3.csv("data/ast%-usg%/cpaul.csv", function (error, csv) {
    csv.forEach(function (d) {
    })
    scatterCPaul = csv;
});

d3.csv("data/ast%-usg%/jkidd.csv", function (error, csv) {
    scatterJKidd = csv;
});

d3.csv("data/ast%-usg%/snash.csv", function (error, csv) {
    scatterSNash = csv;
});

d3.csv("data/ast%-usg%/scurry.csv", function (error, csv) {
    scatterSCurry = csv;
    updateScatterPlot();
});
