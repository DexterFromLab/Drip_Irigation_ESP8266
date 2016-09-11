var szerokosc_okna_wykresu = 0;
var label_dat = [];
series_dat = [];
function fillelements(data) {
	//var tmp = (data.day).toString()+"."+(data.month).toString()+"."+(data.year).toString()+" "+(data.hour).toString()+":"+(data.minute).toString()+":"+(data.second).toString();
	var tmp = new Date(data.year, data.month, data.day, data.hour, data.minute, data.second);
	//$("#time").text(tmp);
	//$("#temp").text(data.temperatura);
	series_dat = 
		[data.t0, data.t1, data.t2, data.t3, data.t4, data.t5, data.t6, data.t7, data.t8, data.t9, data.t10,
			data.t11, data.t12, data.t13, data.t14, data.t15, data.t16, data.t17, data.t18, data.t19, data.t20,
			data.t21, data.t22, data.t23, data.t24, data.t25, data.t26, data.t27, data.t28, data.t29, data.t30,
			data.t31, data.t32, data.t33, data.t34, data.t35, data.t36, data.t37, data.t38, data.t39, data.t40,
			data.t41, data.t42, data.t43, data.t44, data.t45, data.t46, data.t47, data.t48, data.t49, data.t50,
			data.t51, data.t52, data.t53, data.t54, data.t55, data.t56, data.t57, data.t58, data.t59, data.t60,
			data.t61, data.t62, data.t63, data.t64, data.t65, data.t66, data.t67, data.t68, data.t69, data.t70,
			data.t71, data.t72, data.t73, data.t74, data.t75, data.t76, data.t77, data.t78, data.t79, data.t80,
			data.t81, data.t82, data.t83, data.t84, data.t85, data.t86, data.t87, data.t88, data.t89, data.t90,
			data.t91, data.t92, data.t93, data.t94, data.t95, data.t96, data.t97, data.t98, data.t99, data.t100
		]
	
	var szerokosc_wykresu = data.meas_cnt * 3.5
	if(szerokosc_wykresu < 100) szerokosc_wykresu = 100;
	$(".wykres").css("width",szerokosc_wykresu.toString() + "%");
	series_dat = series_dat.slice(0, data.meas_cnt);
	//year,month,day,hour,minute,second,intervall,ilosc_czasow
	var wygenerowane_czasy = GenerateDataStringArrayBack(data.last_meas_time0,data.last_meas_time1,data.last_meas_time2,data.last_meas_time3,data.last_meas_time4,data.last_meas_time5,data.measure_intervall,data.meas_cnt);
	label_dat = wygenerowane_czasy.reverse();
	}
	
	function getMeasuresValues() {
		$.ajax({
			type: "GET",
			datatype: "html",
			url: "/all",
			success: function(response) {
				fillelements(response);
			}
		});
	}
function GenerateDataStringArrayBack(year,month,day,hour,minute,second,intervall,ilosc_czasow){
times = []
year = Number(year);
month = Number(month);
day = Number(day);
hour = Number(hour);
minute = Number(minute);
second = Number(second);
intervall = Number(intervall);
szerokosc_okna_wykresu = ilosc_czasow *30;
if(szerokosc_okna_wykresu<500) szerokosc_okna_wykresu = 500;
for(var i = 0;i<ilosc_czasow;i++){

		times.push((parseInt(hour, 10) + 100).toString().substr(1) + ":" + (parseInt(minute, 10) + 100).toString().substr(1) + ":" + (parseInt(second, 10) + 100).toString().substr(1)+ " " + (parseInt(day, 10) + 100).toString().substr(1) + "-" + (parseInt(month, 10) + 100).toString().substr(1) + "-" + year.toString());

		second = second - intervall
		var monthLen = [31,28,31,30,31,30,31,31,30,31,30,31]
		
		if(((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
		   monthLen[1] = 29;
		else   
		   monthLen[1] = 28;
		
		while(second < 0){
			if(second < 0){
				second += 60;
				minute--;
			}
			if(minute < 0){
				minute += 60;
				hour--;
			}
			if(hour < 0){
				hour += 24;
				day--;
			}
			if(day <= 0){
				month--;
				if(month <= 0){
					month += 12;
					year--;
				}
				day += ((monthLen[month-1]));
			}
		}
	}
return times;
}
getMeasuresValues();
setTimeout(function(){getMeasuresValues();},30000)

var randomScalingFactor = function(){ return Math.round(Math.random()*100)};
var lineChartData = {
labels : label_dat,
datasets : [
	{
		label: "Pomiar temperatury",
		fillColor : "hsla(180,100%,50%,0.2)",
		strokeColor : "hsla(180,100%,50%,1)",
		pointColor : "hsla(180,100%,50%,1)",
		pointStrokeColor : "#fff",
		pointHighlightFill : "#fff",
		pointHighlightStroke : "rgba(220,220,220,1)",
		data : series_dat
	}
]

}
function generateLineChartData(selectedMeasuredTypes,colors,measureData){
	var datasets = [];
	for(var i = 0; i < selectedMeasuredTypes.length;i++){
		var label = selectedMeasuredTypes[i];
		var fillColor = "hsla("+(colors[i][0]).toString()+","+(colors[i][1]).toString()+"%,"+(colors[i][2]).toString()+"%,0.2)";
		var strokeColor = "hsla("+(colors[i][0]).toString()+","+(colors[i][1]).toString()+"%,"+(colors[i][2]).toString()+"%,1)";
		var pointColor = "hsla("+(colors[i][0]).toString()+","+(colors[i][1]).toString()+"%,"+(colors[i][2]).toString()+"%,1)";
		var pointStrokeColor = "#fff";
		var pointHighlightFill = "#fff";
		var pointHighlightStroke = "rgba(220,220,220,1)";
		var data = measureData[i];
		eval('var ob'+(i).toString()+' = new chartConfig(label,fillColor,strokeColor,pointColor,pointStrokeColor,pointHighlightFill,pointHighlightStroke,data);')
		eval('datasets.push(ob'+(i).toString()+')');
	}
	
	return datasets;
}
var chartConfig = function(label,fillColor,strokeColor,pointColor,pointStrokeColor,pointHighlightFill,pointHighlightStroke,data){
	this.label = label;
	this.fillColor = fillColor;
	this.strokeColor = strokeColor;
	this.pointColor = pointColor;
	this.pointStrokeColor = pointStrokeColor;
	this.pointHighlightFill = pointHighlightFill;
	this.pointHighlightStroke = pointHighlightStroke;
	this.data = data;
} 
function drawChart(){
	
	$(".wykres").css("width",szerokosc_okna_wykresu.toString());
	var ctx = document.getElementById("canvas").getContext("2d");
	window.myLine = new Chart(ctx).Line(lineChartData, {
		responsive: true,
		maintainAspectRatio: false
	});
}

function generateColors(num_colors){
	var hue, saturation, lightness;
	var colors = [];
	for(var i = 0; i < 360; i += 360 / num_colors) {
		hue = i;
		saturation = 100;
		lightness = 50;
		colors.push([hue,saturation,lightness]);
	}
	
	return colors;
}
var measuresData = [];
var counterOfGettedMeasures = 0;
function getAllMeasuresData(selectedMeasuredTypes){
	if((counterOfGettedMeasures<selectedMeasuredTypes.length) && (errorGet == 0)){
		initMeasures(selectedMeasuredTypes[counterOfGettedMeasures],pushToMeasureArray)
	}else{
		counterOfGettedMeasures = 0;
		errorGet = 0;
		drawCharts();
	}
	
}
function pushToMeasureArray(data){
	if(counterOfGettedMeasures == 0) measuresData = [];
	measuresData.push(data)
	counterOfGettedMeasures++;
	getAllMeasuresData(selectedMeasuredTypes);
}
function drawCharts(){
	var series_dat = generateLineChartData(selectedMeasuredTypes,generateColors(selectedMeasuredTypes.length),measuresData);
	
	var wygenerowane_czasy = GenerateDataStringArrayBack(lastYear,lastMonth,lastDay,lastHour,lastMinute,lastSecond,measureIntervall,numberOfMeasProbes);
	label_dat = wygenerowane_czasy.reverse();
	
	var lineChartData = {
		labels : label_dat,
		datasets : series_dat

	}
	
	var myNode = document.getElementById("canvas");
	myNode.parentNode.removeChild(myNode);
	$('.wykres').append('<canvas id="canvas"></canvas>');
	
	
	$(".wykres").css("width",szerokosc_okna_wykresu.toString());
	var ctx = document.getElementById("canvas").getContext("2d");
	window.myLine = new Chart(ctx).Line(lineChartData, {
		responsive: true,
		maintainAspectRatio: false
	});
}
var allMeasuredTypes = [];
function getMeasuredConfigAjax(){
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/getMeasuredConfigAjax",
		data: {scriptName: name},
		success: function(response) {
			allMeasuredTypes = response.allMeasuredTypes.split(';')
			//getAllMeasuresData(allMeasuredTypes);
			drawChecboxes(allMeasuredTypes);
			
		},
		error: function(response){
			alert("Data read error!");
		}
	});	
}
function drawChecboxes(names){
	createElement = "";
	for(var i = 0; i<names.length;i++){
		createElement += '<label><input id="'+names[i]+'" type="checkbox" value="">'+names[i]+'</label>';
	}
	$('#checboxMeasuredValues').append(createElement);
}
var selectedMeasuredTypes = [];
function drawCheckedCharts(){
	selectedMeasuredTypes = [];
	for(var i = 0;i<allMeasuredTypes.length;i++){
		if($('#'+allMeasuredTypes[i]).prop('checked') == true) selectedMeasuredTypes.push(allMeasuredTypes[i]);
	}
	getAllMeasuresData(selectedMeasuredTypes);
}