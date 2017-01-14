function ESP8266Reset(){
	$.ajax({
	  method: "GET",
	  url: "reset",
	})
	setTimeout(function(){location.window="index.htm"},30000);
}
function sendInter(){
	
	ValidateIPaddress($("#ip").val(),0);
	ValidateIPaddress($("#m").val(),1);
	ValidateIPaddress($("#gat").val(),2);
	ValidateIPaddress($("#dns1").val(),3);
	ValidateIPaddress($("#dns2").val(),4);
	
	$.ajax({
	  method: "POST",
	  url: "ethernet",
	  data: { 
		ip0: $('#ip').val().split('.')[0], 
		ip1: $("#ip").val().split('.')[1], 
		ip2: $("#ip").val().split('.')[2], 
		ip3: $("#ip").val().split('.')[3], 
		m0: $("#m").val().split('.')[0], 
		m1: $("#m").val().split('.')[1], 
		m2: $("#m").val().split('.')[2], 
		m3: $("#m").val().split('.')[3],
		gat0: $("#gat").val().split('.')[0], 
		gat1: $("#gat").val().split('.')[1], 
		gat2: $("#gat").val().split('.')[2], 
		gat3: $("#gat").val().split('.')[3], 
		dns1_0: $("#dns1").val().split('.')[0], 
		dns1_1: $("#dns1").val().split('.')[1], 
		dns1_2: $("#dns1").val().split('.')[2], 
		dns1_3: $("#dns1").val().split('.')[3],
		dns2_0: $("#dns2").val().split('.')[0], 
		dns2_1: $("#dns2").val().split('.')[1], 
		dns2_2: $("#dns2").val().split('.')[2], 
		dns2_3: $("#dns2").val().split('.')[3], 
		dhcpOn: $("#dhcpOn").val(), 
		siec: $("#wifiChoose").val(), 
		haslo: $("#wifiPass").val()}
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
function sendSys(){
	var VailRes = 0;
	VailRes += VaildRange(0,255,"chandModeHumTimeEnd","Graniczna wartość czasu [s] lub wilgotności[%]");
	if(VailRes) return;
	
	var date = getTimeField();
	$.ajax({
	  method: "POST",
	  url: "system",
	  data: { 
			workMod: $("#workMod").val(),
			chandModeOff: $("#chandModeOff").val(), 
			chandModeHumTimeEnd: $("#chandModeHumTimeEnd").val(), 
			timeMod: $("#timeMod").val(), 
			NTP_addr: $("#NTP_addr").val(),
			year: date[0],
			month: date[1],
			day: date[2],
			hour: date[3],
			minute: date[4],
			second: date[5],
			timeZone: $("#timeZone").val(),
			measInt: parseInt($("#measInt").val())
			}
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
function sendEquation(equation){
	
	$.ajax({
	  method: "POST",
	  url: "parse",
	  data: { Equation: equation.toString() }
	})
	.done(function( msg ) {
		console.log( msg );
	});
}
function sendScriptSettings(){
	var VailRes = 0;
	VailRes += VaildRange(0,65535,"scriptInter","Interwał wykonywania skryptu");
	if(VailRes) return;
	
	$.ajax({
	  method: "POST",
	  url: "scriptSettings",
	  data: { workingScript: $('#scriptSelector').val(), relayScriptTime: $("#scriptInter").val() }
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
var relayScriptTime
function getScriptSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/scriptSettings",
		success: function(response) {
			$("#scriptSelector").val(response.workingScript);
			$("#scriptInter").val(response.relayScriptTime);
			relayScriptTime = response.relayScriptTime;
			$("#scriptName").val(response.workingScript.substr(1));
			getSystemVirables();
		}
	});
}

function getEthernetSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/ethernet",
		success: function(response) {
			$("#ip").val(
				response.ip0.toString() + "."+
				response.ip1.toString() + "."+
				response.ip2.toString() + "."+
				response.ip3.toString()
				);
			$("#m").val(
				response.m0.toString() + "."+
				response.m1.toString() + "."+
				response.m2.toString() + "."+
				response.m3.toString()
			);
			$("#mac").val(
				response.mac0.toString() + ":"+
				response.mac1.toString() + ":"+
				response.mac2.toString() + ":"+
				response.mac3.toString() + ":"+
				response.mac4.toString() + ":"+
				response.mac5.toString()
			);
			$("#gat").val(
				response.gat0.toString() + "."+
				response.gat1.toString() + "."+
				response.gat2.toString() + "."+
				response.gat3.toString()
			);
			$("#dns1").val(
				response.dns1_0.toString() + "."+
				response.dns1_1.toString() + "."+
				response.dns1_2.toString() + "."+
				response.dns1_3.toString()
			);
			$("#dns2").val(
				response.dns2_0.toString() + "."+
				response.dns2_1.toString() + "."+
				response.dns2_2.toString() + "."+
				response.dns2_3.toString()
			);
			$("#dhcpOn").val(response.dhcpOn);
			var i = 0;
			
			var x = document.getElementById("wifiList");
			
			for(i = 0; i<(response.numOfWiFi); i++){
				var option = document.createElement("option");
				option.text = eval("response.name" + i.toString()) +" siła sygnału: "+ eval("response.signal" + i.toString()).toString() + " typ hasła: " + eval("response.encryption" + i.toString());
				option.value = eval("response.name" + i.toString());
				x.add(option);
			}
		}
	});
}
function getSystemSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/system",
		success: function(response) {
			$("#workMod").val(response.workMod);
			$("#chandModeOff").val(response.chandModeOff);
			$("#chandModeHumTimeEnd").val(response.chandModeHumTimeEnd);
			$("#timeMod").val(response.timeMod);
			$("#NTP_addr").val(response.NTP_addr);
			var date = new Array (response.year,response.month,response.day,response.hour,response.minute,response.second)
			fillTimeField(date);
			$("#timeZone").val(response.timeZone);
			$("#measInt").val(response.measInt);
		}
	});
}
function getTimeField() {
	
	var date = new Array(
		Number($('#date').val().split('-')[0]),
		Number($('#date').val().split('-')[1]),
		Number($('#date').val().split('-')[2]),
		Number($('#time').val().split(':')[0]),
		Number($('#time').val().split(':')[1]),
		Number($('#time').val().split(':')[2])
	)
	return date
}


function fillTimeField(date){

	$('#date').val(
		date[0].toString() + "-" +
		padLeft((date[1]),2) + "-" + 
		padLeft(date[2],2)
	);
	$('#time').val(
		padLeft(date[3],2) + ":" +
		padLeft(date[4],2) + ":" +
		padLeft(date[5],2)
	)
}

function padLeft(nr, n, str){
    return Array(n-String(nr).length+1).join(str||'0')+nr;
}

//walidacja

function ValidateIPaddress(ipaddress,typ)   
{  
 if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress))  
  {  
    return (true)  
  }
if(typ == 0)  
	alert("Zła składnia adresu IP");
else if(typ == 1)
	alert("Zła składnia adresu maski podsieci");
else if(typ == 2)
	alert("Zła składnia adresu bramy wyjściowej");
else if(typ == 3)
	alert("Zła składnia adresu DNS1");
else if(typ == 4)
	alert("Zła składnia adresu DNS2");
return (false)  
} 
function VaildRange(min, max, id,typ){
	val = Number($('#'+id).val());
	var mess;
	if(((val<min)|(val>max))){
		if(val<min) {mess = "mała"} else{ mess = "duża"};
		alert("Wartość "+typ+" jest zbyt " + mess + ".");
		return 1;
	}
}


function sendScript(){
	var content = $('#controlScript').val()
	$.ajax({
	  method: "POST",
	  url: "getScriptLine",
	  data: { scriptLine : content },
	  success: function(response) {
		alert("Script " + name + " saved in file system")
		$("#page-wrapper").load( "script.htm" );
	  }, error: function (error) {
        if(name == error.responseText){
			alert("Plik "+ name + " wysłany poprawnie");
			$("#page-wrapper").load( "script.htm" );
		}else{
			alert("Błąd wysyłania danych");
		}
      }
	})
}
var name 
function getName(){
	name = "/"
	name += $('#scriptName').val()
	if(name.split(".").pop() == "scr"){
		$.ajax({
		  method: "POST",
		  url: "getScriptName",
		  data: { scriptName : name },
		  success: function(response) {
			sendScript();
		  },
		  error: function (error) {
			if(name == error.responseText){
				sendScript();
			}else{
				alert("Błąd wysyłania danych");
			}
		  }
		})
	}else{
		alert("Zły format pliku, nazwa musi być zakończona .scr");
	}
}
function getScriptsNames(){
	var z=0;
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/getScriptsNames",
		success: function(response) {
			for(z ; z<response.length ; z++){
				var x = document.getElementById("scriptSelector");
					var option = document.createElement("option");
					option.text = response[z].name;
					option.value = response[z].name;
					x.add(option);
			}
		}
	});	
}
function deleteFile(name){
	$.ajax({
		type: "POST",
		datatype: "html",
		url: "/deleteFile",
		data: {file: name},
		success: function(response) {
			$("#page-wrapper").load( "script.htm" );
		}
	});	
}
function getScriptContent(name){
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/getScriptContent",
		data: {scriptName: name},
		success: function(response) {
			$("#controlScript").text(response.content)
		},
		error: function(response){
			$("#controlScript").text(response.responseText)			
		}
	});	
}

var firstInit = 0;
function getSystemVirables(){
		getSystemVirablesAjax();
		if(firstInit == 0){
			setInterval(function(){
				if(!document.getElementById("controlScript")==false){
					getSystemVirablesAjax();
				}
			}, relayScriptTime*1000);
			firstInit++;
		}
}
function getSystemVirablesAjax(){
		$.ajax({
			type: "GET",
			datatype: "html",
			url: "/systemVirablesValues",
			success: function(response) {
				var string = "Zmienne wejściowe\n";
				for(var i = 0;i<response.length;i++){
					string += response[i].n + ": " + response[i].v + "\n";
				}
				$("#scriptVirables").text(string);
			},
			error: function(response){
				
			}
		});	
		$.ajax({
			type: "GET",
			datatype: "html",
			url: "/systemOutVirablesValues",
			success: function(response) {
				var string = "Zmienne wyjściowe\n";
				for(var i = 0;i<response.length;i++){
					string += response[i].n + ": " + response[i].v + "\n";
				}
				$("#scriptOutVirables").text(string);
			},
			error: function(response){
				
			}
		});
}
var allRelayNames = []
var allRelayValues = []
var allRelayControlState = []
var allVirablesNames = []
var allVirablesValues = []
function getSystemVirablesForInfo(){
		allRelayNames = []
		allRelayValues = []
		$.ajax({
			type: "GET",
			datatype: "html",
			url: "/systemOutVirablesValues",
			success: function(response) {
				for(var i = 0;i<response.length;i++){
					if((response[i].n.indexOf("Relay") != -1)){
						allRelayNames.push(response[i].n);
						allRelayValues.push(response[i].v);
					}
					if((response[i].n.indexOf("AutoControl") != -1)){
							allRelayControlState.push(response[i].v);
						}					
				}
				getValuesAfter();
			},
			error: function(response){
				
			}
		});		
		allVirablesNames = []
		allVirablesValues = []
		function getValuesAfter(){
			$.ajax({
				type: "GET",
				datatype: "html",
				url: "/systemVirablesValues",
				success: function(response) {
					for(var i = 0;i<response.length;i++){
						if((response[i].n.indexOf("Temp") != -1)||(response[i].n.indexOf("Hum") != -1)||(response[i].n.indexOf("Soil") != -1)){
							allVirablesNames.push(response[i].n);
							allVirablesValues.push(response[i].v);
						}
					}
					fillTailsInfo();
				},
				error: function(response){
					
				}
			});
		}
}
function fillTailsInfo(){

	var htmlString = ""

	for(var i = 0;i < allVirablesNames.length; i++){
		eval('var '+allVirablesNames[i]+' = new kafelek("'+allVirablesNames[i]+'");');
		var type = 0; 
		if(allVirablesNames[i].indexOf("Temp") != -1) type = 1;
		if(allVirablesNames[i].indexOf("Hum") != -1) type = 2;
		if(allVirablesNames[i].indexOf("Soil") != -1) type = 3;
		htmlString += eval(allVirablesNames[i] +'.draw('+type+')');
	}
	for(var i = 0;i < allRelayNames.length; i++){
		eval('var '+allRelayNames[i]+' = new kafelek("'+allRelayNames[i]+'");');
		htmlString += eval(allRelayNames[i] +'.draw(0)')
	}
	$("#plane").html(htmlString);



	for(var i = 0;i < allRelayNames.length; i++){
		eval(allRelayNames[i] + '.toggleColorState('+Number(allRelayValues[i])+',0);');
	}
	for(var i = 0;i < allVirablesNames.length; i++){
		eval(allVirablesNames[i]+'.setVal('+Number(allVirablesValues[i])+');');
	}	
}

/*!
 * Chart.js
 * http://chartjs.org/
 * Version: 1.1.1
 *
 * Copyright 2015 Nick Downie
 * Released under the MIT license
 * https://github.com/nnnick/Chart.js/blob/master/LICENSE.md
 */
(function(){"use strict";var t=this,i=t.Chart,e=function(t){this.canvas=t.canvas,this.ctx=t;var i=function(t,i){return t["offset"+i]?t["offset"+i]:document.defaultView.getComputedStyle(t).getPropertyValue(i)};this.width=i(t.canvas,"Width")||t.canvas.width,this.height=i(t.canvas,"Height")||t.canvas.height;return this.aspectRatio=this.width/this.height,s.retinaScale(this),this};e.defaults={global:{animation:!0,animationSteps:60,animationEasing:"easeOutQuart",showScale:!0,scaleOverride:!1,scaleSteps:null,scaleStepWidth:null,scaleStartValue:null,scaleLineColor:"rgba(0,0,0,.1)",scaleLineWidth:1,scaleShowLabels:!0,scaleLabel:"<%=value%>",scaleIntegersOnly:!0,scaleBeginAtZero:!1,scaleFontFamily:"'Helvetica Neue', 'Helvetica', 'Arial', sans-serif",scaleFontSize:12,scaleFontStyle:"normal",scaleFontColor:"#666",responsive:!1,maintainAspectRatio:!0,showTooltips:!0,customTooltips:!1,tooltipEvents:["mousemove","touchstart","touchmove","mouseout"],tooltipFillColor:"rgba(0,0,0,0.8)",tooltipFontFamily:"'Helvetica Neue', 'Helvetica', 'Arial', sans-serif",tooltipFontSize:14,tooltipFontStyle:"normal",tooltipFontColor:"#fff",tooltipTitleFontFamily:"'Helvetica Neue', 'Helvetica', 'Arial', sans-serif",tooltipTitleFontSize:14,tooltipTitleFontStyle:"bold",tooltipTitleFontColor:"#fff",tooltipTitleTemplate:"<%= label%>",tooltipYPadding:6,tooltipXPadding:6,tooltipCaretSize:8,tooltipCornerRadius:6,tooltipXOffset:10,tooltipTemplate:"<%if (label){%><%=label%>: <%}%><%= value %>",multiTooltipTemplate:"<%= datasetLabel %>: <%= value %>",multiTooltipKeyBackground:"#fff",segmentColorDefault:["#A6CEE3","#1F78B4","#B2DF8A","#33A02C","#FB9A99","#E31A1C","#FDBF6F","#FF7F00","#CAB2D6","#6A3D9A","#B4B482","#B15928"],segmentHighlightColorDefaults:["#CEF6FF","#47A0DC","#DAFFB2","#5BC854","#FFC2C1","#FF4244","#FFE797","#FFA728","#F2DAFE","#9265C2","#DCDCAA","#D98150"],onAnimationProgress:function(){},onAnimationComplete:function(){}}},e.types={};var s=e.helpers={},n=s.each=function(t,i,e){var s=Array.prototype.slice.call(arguments,3);if(t)if(t.length===+t.length){var n;for(n=0;n<t.length;n++)i.apply(e,[t[n],n].concat(s))}else for(var o in t)i.apply(e,[t[o],o].concat(s))},o=s.clone=function(t){var i={};return n(t,function(e,s){t.hasOwnProperty(s)&&(i[s]=e)}),i},a=s.extend=function(t){return n(Array.prototype.slice.call(arguments,1),function(i){n(i,function(e,s){i.hasOwnProperty(s)&&(t[s]=e)})}),t},h=s.merge=function(t,i){var e=Array.prototype.slice.call(arguments,0);return e.unshift({}),a.apply(null,e)},l=s.indexOf=function(t,i){if(Array.prototype.indexOf)return t.indexOf(i);for(var e=0;e<t.length;e++)if(t[e]===i)return e;return-1},r=(s.where=function(t,i){var e=[];return s.each(t,function(t){i(t)&&e.push(t)}),e},s.findNextWhere=function(t,i,e){e||(e=-1);for(var s=e+1;s<t.length;s++){var n=t[s];if(i(n))return n}},s.findPreviousWhere=function(t,i,e){e||(e=t.length);for(var s=e-1;s>=0;s--){var n=t[s];if(i(n))return n}},s.inherits=function(t){var i=this,e=t&&t.hasOwnProperty("constructor")?t.constructor:function(){return i.apply(this,arguments)},s=function(){this.constructor=e};return s.prototype=i.prototype,e.prototype=new s,e.extend=r,t&&a(e.prototype,t),e.__super__=i.prototype,e}),c=s.noop=function(){},u=s.uid=function(){var t=0;return function(){return"chart-"+t++}}(),d=s.warn=function(t){window.console&&"function"==typeof window.console.warn&&console.warn(t)},p=s.amd="function"==typeof define&&define.amd,f=s.isNumber=function(t){return!isNaN(parseFloat(t))&&isFinite(t)},g=s.max=function(t){return Math.max.apply(Math,t)},m=s.min=function(t){return Math.min.apply(Math,t)},v=(s.cap=function(t,i,e){if(f(i)){if(t>i)return i}else if(f(e)&&e>t)return e;return t},s.getDecimalPlaces=function(t){if(t%1!==0&&f(t)){var i=t.toString();if(i.indexOf("e-")<0)return i.split(".")[1].length;if(i.indexOf(".")<0)return parseInt(i.split("e-")[1]);var e=i.split(".")[1].split("e-");return e[0].length+parseInt(e[1])}return 0}),S=s.radians=function(t){return t*(Math.PI/180)},C=(s.getAngleFromPoint=function(t,i){var e=i.x-t.x,s=i.y-t.y,n=Math.sqrt(e*e+s*s),o=2*Math.PI+Math.atan2(s,e);return 0>e&&0>s&&(o+=2*Math.PI),{angle:o,distance:n}},s.aliasPixel=function(t){return t%2===0?0:.5}),x=(s.splineCurve=function(t,i,e,s){var n=Math.sqrt(Math.pow(i.x-t.x,2)+Math.pow(i.y-t.y,2)),o=Math.sqrt(Math.pow(e.x-i.x,2)+Math.pow(e.y-i.y,2)),a=s*n/(n+o),h=s*o/(n+o);return{inner:{x:i.x-a*(e.x-t.x),y:i.y-a*(e.y-t.y)},outer:{x:i.x+h*(e.x-t.x),y:i.y+h*(e.y-t.y)}}},s.calculateOrderOfMagnitude=function(t){return Math.floor(Math.log(t)/Math.LN10)}),y=(s.calculateScaleRange=function(t,i,e,s,o){var a=2,h=Math.floor(i/(1.5*e)),l=a>=h,r=[];n(t,function(t){null==t||r.push(t)});var c=m(r),u=g(r);u===c&&(u+=.5,c>=.5&&!s?c-=.5:u+=.5);for(var d=Math.abs(u-c),p=x(d),f=Math.ceil(u/(1*Math.pow(10,p)))*Math.pow(10,p),v=s?0:Math.floor(c/(1*Math.pow(10,p)))*Math.pow(10,p),S=f-v,C=Math.pow(10,p),y=Math.round(S/C);(y>h||h>2*y)&&!l;)if(y>h)C*=2,y=Math.round(S/C),y%1!==0&&(l=!0);else if(o&&p>=0){if(C/2%1!==0)break;C/=2,y=Math.round(S/C)}else C/=2,y=Math.round(S/C);return l&&(y=a,C=S/y),{steps:y,stepValue:C,min:v,max:v+y*C}},s.template=function(t,i){function e(t,i){var e=/\W/.test(t)?new Function("obj","var p=[],print=function(){p.push.apply(p,arguments);};with(obj){p.push('"+t.replace(/[\r\t\n]/g," ").split("<%").join("	").replace(/((^|%>)[^\t]*)'/g,"$1\r").replace(/\t=(.*?)%>/g,"',$1,'").split("	").join("');").split("%>").join("p.push('").split("\r").join("\\'")+"');}return p.join('');"):s[t]=s[t];return i?e(i):e}if(t instanceof Function)return t(i);var s={};return e(t,i)}),b=(s.generateLabels=function(t,i,e,s){var o=new Array(i);return t&&n(o,function(i,n){o[n]=y(t,{value:e+s*(n+1)})}),o},s.easingEffects={linear:function(t){return t},easeInQuad:function(t){return t*t},easeOutQuad:function(t){return-1*t*(t-2)},easeInOutQuad:function(t){return(t/=.5)<1?.5*t*t:-0.5*(--t*(t-2)-1)},easeInCubic:function(t){return t*t*t},easeOutCubic:function(t){return 1*((t=t/1-1)*t*t+1)},easeInOutCubic:function(t){return(t/=.5)<1?.5*t*t*t:.5*((t-=2)*t*t+2)},easeInQuart:function(t){return t*t*t*t},easeOutQuart:function(t){return-1*((t=t/1-1)*t*t*t-1)},easeInOutQuart:function(t){return(t/=.5)<1?.5*t*t*t*t:-0.5*((t-=2)*t*t*t-2)},easeInQuint:function(t){return 1*(t/=1)*t*t*t*t},easeOutQuint:function(t){return 1*((t=t/1-1)*t*t*t*t+1)},easeInOutQuint:function(t){return(t/=.5)<1?.5*t*t*t*t*t:.5*((t-=2)*t*t*t*t+2)},easeInSine:function(t){return-1*Math.cos(t/1*(Math.PI/2))+1},easeOutSine:function(t){return 1*Math.sin(t/1*(Math.PI/2))},easeInOutSine:function(t){return-0.5*(Math.cos(Math.PI*t/1)-1)},easeInExpo:function(t){return 0===t?1:1*Math.pow(2,10*(t/1-1))},easeOutExpo:function(t){return 1===t?1:1*(-Math.pow(2,-10*t/1)+1)},easeInOutExpo:function(t){return 0===t?0:1===t?1:(t/=.5)<1?.5*Math.pow(2,10*(t-1)):.5*(-Math.pow(2,-10*--t)+2)},easeInCirc:function(t){return t>=1?t:-1*(Math.sqrt(1-(t/=1)*t)-1)},easeOutCirc:function(t){return 1*Math.sqrt(1-(t=t/1-1)*t)},easeInOutCirc:function(t){return(t/=.5)<1?-0.5*(Math.sqrt(1-t*t)-1):.5*(Math.sqrt(1-(t-=2)*t)+1)},easeInElastic:function(t){var i=1.70158,e=0,s=1;return 0===t?0:1==(t/=1)?1:(e||(e=.3),s<Math.abs(1)?(s=1,i=e/4):i=e/(2*Math.PI)*Math.asin(1/s),-(s*Math.pow(2,10*(t-=1))*Math.sin((1*t-i)*(2*Math.PI)/e)))},easeOutElastic:function(t){var i=1.70158,e=0,s=1;return 0===t?0:1==(t/=1)?1:(e||(e=.3),s<Math.abs(1)?(s=1,i=e/4):i=e/(2*Math.PI)*Math.asin(1/s),s*Math.pow(2,-10*t)*Math.sin((1*t-i)*(2*Math.PI)/e)+1)},easeInOutElastic:function(t){var i=1.70158,e=0,s=1;return 0===t?0:2==(t/=.5)?1:(e||(e=1*(.3*1.5)),s<Math.abs(1)?(s=1,i=e/4):i=e/(2*Math.PI)*Math.asin(1/s),1>t?-.5*(s*Math.pow(2,10*(t-=1))*Math.sin((1*t-i)*(2*Math.PI)/e)):s*Math.pow(2,-10*(t-=1))*Math.sin((1*t-i)*(2*Math.PI)/e)*.5+1)},easeInBack:function(t){var i=1.70158;return 1*(t/=1)*t*((i+1)*t-i)},easeOutBack:function(t){var i=1.70158;return 1*((t=t/1-1)*t*((i+1)*t+i)+1)},easeInOutBack:function(t){var i=1.70158;return(t/=.5)<1?.5*(t*t*(((i*=1.525)+1)*t-i)):.5*((t-=2)*t*(((i*=1.525)+1)*t+i)+2)},easeInBounce:function(t){return 1-b.easeOutBounce(1-t)},easeOutBounce:function(t){return(t/=1)<1/2.75?1*(7.5625*t*t):2/2.75>t?1*(7.5625*(t-=1.5/2.75)*t+.75):2.5/2.75>t?1*(7.5625*(t-=2.25/2.75)*t+.9375):1*(7.5625*(t-=2.625/2.75)*t+.984375)},easeInOutBounce:function(t){return.5>t?.5*b.easeInBounce(2*t):.5*b.easeOutBounce(2*t-1)+.5}}),w=s.requestAnimFrame=function(){return window.requestAnimationFrame||window.webkitRequestAnimationFrame||window.mozRequestAnimationFrame||window.oRequestAnimationFrame||window.msRequestAnimationFrame||function(t){return window.setTimeout(t,1e3/60)}}(),P=(s.cancelAnimFrame=function(){return window.cancelAnimationFrame||window.webkitCancelAnimationFrame||window.mozCancelAnimationFrame||window.oCancelAnimationFrame||window.msCancelAnimationFrame||function(t){return window.clearTimeout(t,1e3/60)}}(),s.animationLoop=function(t,i,e,s,n,o){var a=0,h=b[e]||b.linear,l=function(){a++;var e=a/i,r=h(e);t.call(o,r,e,a),s.call(o,r,e),i>a?o.animationFrame=w(l):n.apply(o)};w(l)},s.getRelativePosition=function(t){var i,e,s=t.originalEvent||t,n=t.currentTarget||t.srcElement,o=n.getBoundingClientRect();return s.touches?(i=s.touches[0].clientX-o.left,e=s.touches[0].clientY-o.top):(i=s.clientX-o.left,e=s.clientY-o.top),{x:i,y:e}},s.addEvent=function(t,i,e){t.addEventListener?t.addEventListener(i,e):t.attachEvent?t.attachEvent("on"+i,e):t["on"+i]=e}),L=s.removeEvent=function(t,i,e){t.removeEventListener?t.removeEventListener(i,e,!1):t.detachEvent?t.detachEvent("on"+i,e):t["on"+i]=c},k=(s.bindEvents=function(t,i,e){t.events||(t.events={}),n(i,function(i){t.events[i]=function(){e.apply(t,arguments)},P(t.chart.canvas,i,t.events[i])})},s.unbindEvents=function(t,i){n(i,function(i,e){L(t.chart.canvas,e,i)})}),F=s.getMaximumWidth=function(t){var i=t.parentNode,e=parseInt(R(i,"padding-left"))+parseInt(R(i,"padding-right"));return i?i.clientWidth-e:0},A=s.getMaximumHeight=function(t){var i=t.parentNode,e=parseInt(R(i,"padding-bottom"))+parseInt(R(i,"padding-top"));return i?i.clientHeight-e:0},R=s.getStyle=function(t,i){return t.currentStyle?t.currentStyle[i]:document.defaultView.getComputedStyle(t,null).getPropertyValue(i)},T=(s.getMaximumSize=s.getMaximumWidth,s.retinaScale=function(t){var i=t.ctx,e=t.canvas.width,s=t.canvas.height;window.devicePixelRatio&&(i.canvas.style.width=e+"px",i.canvas.style.height=s+"px",i.canvas.height=s*window.devicePixelRatio,i.canvas.width=e*window.devicePixelRatio,i.scale(window.devicePixelRatio,window.devicePixelRatio))}),M=s.clear=function(t){t.ctx.clearRect(0,0,t.width,t.height)},W=s.fontString=function(t,i,e){return i+" "+t+"px "+e},z=s.longestText=function(t,i,e){t.font=i;var s=0;return n(e,function(i){var e=t.measureText(i).width;s=e>s?e:s}),s},B=s.drawRoundedRectangle=function(t,i,e,s,n,o){t.beginPath(),t.moveTo(i+o,e),t.lineTo(i+s-o,e),t.quadraticCurveTo(i+s,e,i+s,e+o),t.lineTo(i+s,e+n-o),t.quadraticCurveTo(i+s,e+n,i+s-o,e+n),t.lineTo(i+o,e+n),t.quadraticCurveTo(i,e+n,i,e+n-o),t.lineTo(i,e+o),t.quadraticCurveTo(i,e,i+o,e),t.closePath()};e.instances={},e.Type=function(t,i,s){this.options=i,this.chart=s,this.id=u(),e.instances[this.id]=this,i.responsive&&this.resize(),this.initialize.call(this,t)},a(e.Type.prototype,{initialize:function(){return this},clear:function(){return M(this.chart),this},stop:function(){return e.animationService.cancelAnimation(this),this},resize:function(t){this.stop();var i=this.chart.canvas,e=F(this.chart.canvas),s=this.options.maintainAspectRatio?e/this.chart.aspectRatio:A(this.chart.canvas);return i.width=this.chart.width=e,i.height=this.chart.height=s,T(this.chart),"function"==typeof t&&t.apply(this,Array.prototype.slice.call(arguments,1)),this},reflow:c,render:function(t){if(t&&this.reflow(),this.options.animation&&!t){var i=new e.Animation;i.numSteps=this.options.animationSteps,i.easing=this.options.animationEasing,i.render=function(t,i){var e=s.easingEffects[i.easing],n=i.currentStep/i.numSteps,o=e(n);t.draw(o,n,i.currentStep)},i.onAnimationProgress=this.options.onAnimationProgress,i.onAnimationComplete=this.options.onAnimationComplete,e.animationService.addAnimation(this,i)}else this.draw(),this.options.onAnimationComplete.call(this);return this},generateLegend:function(){return s.template(this.options.legendTemplate,this)},destroy:function(){this.stop(),this.clear(),k(this,this.events);var t=this.chart.canvas;t.width=this.chart.width,t.height=this.chart.height,t.style.removeProperty?(t.style.removeProperty("width"),t.style.removeProperty("height")):(t.style.removeAttribute("width"),t.style.removeAttribute("height")),delete e.instances[this.id]},showTooltip:function(t,i){"undefined"==typeof this.activeElements&&(this.activeElements=[]);var o=function(t){var i=!1;return t.length!==this.activeElements.length?i=!0:(n(t,function(t,e){t!==this.activeElements[e]&&(i=!0)},this),i)}.call(this,t);if(o||i){if(this.activeElements=t,this.draw(),this.options.customTooltips&&this.options.customTooltips(!1),t.length>0)if(this.datasets&&this.datasets.length>1){for(var a,h,r=this.datasets.length-1;r>=0&&(a=this.datasets[r].points||this.datasets[r].bars||this.datasets[r].segments,h=l(a,t[0]),-1===h);r--);var c=[],u=[],d=function(t){var i,e,n,o,a,l=[],r=[],d=[];return s.each(this.datasets,function(t){i=t.points||t.bars||t.segments,i[h]&&i[h].hasValue()&&l.push(i[h])}),s.each(l,function(t){r.push(t.x),d.push(t.y),c.push(s.template(this.options.multiTooltipTemplate,t)),u.push({fill:t._saved.fillColor||t.fillColor,stroke:t._saved.strokeColor||t.strokeColor})},this),a=m(d),n=g(d),o=m(r),e=g(r),{x:o>this.chart.width/2?o:e,y:(a+n)/2}}.call(this,h);new e.MultiTooltip({x:d.x,y:d.y,xPadding:this.options.tooltipXPadding,yPadding:this.options.tooltipYPadding,xOffset:this.options.tooltipXOffset,fillColor:this.options.tooltipFillColor,textColor:this.options.tooltipFontColor,fontFamily:this.options.tooltipFontFamily,fontStyle:this.options.tooltipFontStyle,fontSize:this.options.tooltipFontSize,titleTextColor:this.options.tooltipTitleFontColor,titleFontFamily:this.options.tooltipTitleFontFamily,titleFontStyle:this.options.tooltipTitleFontStyle,titleFontSize:this.options.tooltipTitleFontSize,cornerRadius:this.options.tooltipCornerRadius,labels:c,legendColors:u,legendColorBackground:this.options.multiTooltipKeyBackground,title:y(this.options.tooltipTitleTemplate,t[0]),chart:this.chart,ctx:this.chart.ctx,custom:this.options.customTooltips}).draw()}else n(t,function(t){var i=t.tooltipPosition();new e.Tooltip({x:Math.round(i.x),y:Math.round(i.y),xPadding:this.options.tooltipXPadding,yPadding:this.options.tooltipYPadding,fillColor:this.options.tooltipFillColor,textColor:this.options.tooltipFontColor,fontFamily:this.options.tooltipFontFamily,fontStyle:this.options.tooltipFontStyle,fontSize:this.options.tooltipFontSize,caretHeight:this.options.tooltipCaretSize,cornerRadius:this.options.tooltipCornerRadius,text:y(this.options.tooltipTemplate,t),chart:this.chart,custom:this.options.customTooltips}).draw()},this);return this}},toBase64Image:function(){return this.chart.canvas.toDataURL.apply(this.chart.canvas,arguments)}}),e.Type.extend=function(t){var i=this,s=function(){return i.apply(this,arguments)};if(s.prototype=o(i.prototype),a(s.prototype,t),s.extend=e.Type.extend,t.name||i.prototype.name){var n=t.name||i.prototype.name,l=e.defaults[i.prototype.name]?o(e.defaults[i.prototype.name]):{};e.defaults[n]=a(l,t.defaults),e.types[n]=s,e.prototype[n]=function(t,i){var o=h(e.defaults.global,e.defaults[n],i||{});return new s(t,o,this)}}else d("Name not provided for this chart, so it hasn't been registered");return i},e.Element=function(t){a(this,t),this.initialize.apply(this,arguments),this.save()},a(e.Element.prototype,{initialize:function(){},restore:function(t){return t?n(t,function(t){this[t]=this._saved[t]},this):a(this,this._saved),this},save:function(){return this._saved=o(this),delete this._saved._saved,this},update:function(t){return n(t,function(t,i){this._saved[i]=this[i],this[i]=t},this),this},transition:function(t,i){return n(t,function(t,e){this[e]=(t-this._saved[e])*i+this._saved[e]},this),this},tooltipPosition:function(){return{x:this.x,y:this.y}},hasValue:function(){return f(this.value)}}),e.Element.extend=r,e.Point=e.Element.extend({display:!0,inRange:function(t,i){var e=this.hitDetectionRadius+this.radius;return Math.pow(t-this.x,2)+Math.pow(i-this.y,2)<Math.pow(e,2)},draw:function(){if(this.display){var t=this.ctx;t.beginPath(),t.arc(this.x,this.y,this.radius,0,2*Math.PI),t.closePath(),t.strokeStyle=this.strokeColor,t.lineWidth=this.strokeWidth,t.fillStyle=this.fillColor,t.fill(),t.stroke()}}}),e.Arc=e.Element.extend({inRange:function(t,i){var e=s.getAngleFromPoint(this,{x:t,y:i}),n=e.angle%(2*Math.PI),o=(2*Math.PI+this.startAngle)%(2*Math.PI),a=(2*Math.PI+this.endAngle)%(2*Math.PI)||360,h=o>a?a>=n||n>=o:n>=o&&a>=n,l=e.distance>=this.innerRadius&&e.distance<=this.outerRadius;return h&&l},tooltipPosition:function(){var t=this.startAngle+(this.endAngle-this.startAngle)/2,i=(this.outerRadius-this.innerRadius)/2+this.innerRadius;return{x:this.x+Math.cos(t)*i,y:this.y+Math.sin(t)*i}},draw:function(t){var i=this.ctx;i.beginPath(),i.arc(this.x,this.y,this.outerRadius<0?0:this.outerRadius,this.startAngle,this.endAngle),i.arc(this.x,this.y,this.innerRadius<0?0:this.innerRadius,this.endAngle,this.startAngle,!0),i.closePath(),i.strokeStyle=this.strokeColor,i.lineWidth=this.strokeWidth,i.fillStyle=this.fillColor,i.fill(),i.lineJoin="bevel",this.showStroke&&i.stroke()}}),e.Rectangle=e.Element.extend({draw:function(){var t=this.ctx,i=this.width/2,e=this.x-i,s=this.x+i,n=this.base-(this.base-this.y),o=this.strokeWidth/2;this.showStroke&&(e+=o,s-=o,n+=o),t.beginPath(),t.fillStyle=this.fillColor,t.strokeStyle=this.strokeColor,t.lineWidth=this.strokeWidth,t.moveTo(e,this.base),t.lineTo(e,n),t.lineTo(s,n),t.lineTo(s,this.base),t.fill(),this.showStroke&&t.stroke()},height:function(){return this.base-this.y},inRange:function(t,i){return t>=this.x-this.width/2&&t<=this.x+this.width/2&&i>=this.y&&i<=this.base}}),e.Animation=e.Element.extend({currentStep:null,numSteps:60,easing:"",render:null,onAnimationProgress:null,onAnimationComplete:null}),e.Tooltip=e.Element.extend({draw:function(){var t=this.chart.ctx;t.font=W(this.fontSize,this.fontStyle,this.fontFamily),this.xAlign="center",this.yAlign="above";var i=this.caretPadding=2,e=t.measureText(this.text).width+2*this.xPadding,s=this.fontSize+2*this.yPadding,n=s+this.caretHeight+i;this.x+e/2>this.chart.width?this.xAlign="left":this.x-e/2<0&&(this.xAlign="right"),this.y-n<0&&(this.yAlign="below");var o=this.x-e/2,a=this.y-n;if(t.fillStyle=this.fillColor,this.custom)this.custom(this);else{switch(this.yAlign){case"above":t.beginPath(),t.moveTo(this.x,this.y-i),t.lineTo(this.x+this.caretHeight,this.y-(i+this.caretHeight)),t.lineTo(this.x-this.caretHeight,this.y-(i+this.caretHeight)),t.closePath(),t.fill();break;case"below":a=this.y+i+this.caretHeight,t.beginPath(),t.moveTo(this.x,this.y+i),t.lineTo(this.x+this.caretHeight,this.y+i+this.caretHeight),t.lineTo(this.x-this.caretHeight,this.y+i+this.caretHeight),t.closePath(),t.fill()}switch(this.xAlign){case"left":o=this.x-e+(this.cornerRadius+this.caretHeight);break;case"right":o=this.x-(this.cornerRadius+this.caretHeight)}B(t,o,a,e,s,this.cornerRadius),t.fill(),t.fillStyle=this.textColor,t.textAlign="center",t.textBaseline="middle",t.fillText(this.text,o+e/2,a+s/2)}}}),e.MultiTooltip=e.Element.extend({initialize:function(){this.font=W(this.fontSize,this.fontStyle,this.fontFamily),this.titleFont=W(this.titleFontSize,this.titleFontStyle,this.titleFontFamily),this.titleHeight=this.title?1.5*this.titleFontSize:0,this.height=this.labels.length*this.fontSize+(this.labels.length-1)*(this.fontSize/2)+2*this.yPadding+this.titleHeight,this.ctx.font=this.titleFont;var t=this.ctx.measureText(this.title).width,i=z(this.ctx,this.font,this.labels)+this.fontSize+3,e=g([i,t]);this.width=e+2*this.xPadding;var s=this.height/2;this.y-s<0?this.y=s:this.y+s>this.chart.height&&(this.y=this.chart.height-s),this.x>this.chart.width/2?this.x-=this.xOffset+this.width:this.x+=this.xOffset},getLineHeight:function(t){var i=this.y-this.height/2+this.yPadding,e=t-1;return 0===t?i+this.titleHeight/3:i+(1.5*this.fontSize*e+this.fontSize/2)+this.titleHeight},draw:function(){if(this.custom)this.custom(this);else{B(this.ctx,this.x,this.y-this.height/2,this.width,this.height,this.cornerRadius);var t=this.ctx;t.fillStyle=this.fillColor,t.fill(),t.closePath(),t.textAlign="left",t.textBaseline="middle",t.fillStyle=this.titleTextColor,t.font=this.titleFont,t.fillText(this.title,this.x+this.xPadding,this.getLineHeight(0)),t.font=this.font,s.each(this.labels,function(i,e){t.fillStyle=this.textColor,t.fillText(i,this.x+this.xPadding+this.fontSize+3,this.getLineHeight(e+1)),t.fillStyle=this.legendColorBackground,t.fillRect(this.x+this.xPadding,this.getLineHeight(e+1)-this.fontSize/2,this.fontSize,this.fontSize),t.fillStyle=this.legendColors[e].fill,t.fillRect(this.x+this.xPadding,this.getLineHeight(e+1)-this.fontSize/2,this.fontSize,this.fontSize)},this)}}}),e.Scale=e.Element.extend({initialize:function(){this.fit()},buildYLabels:function(){this.yLabels=[];for(var t=v(this.stepValue),i=0;i<=this.steps;i++)this.yLabels.push(y(this.templateString,{value:(this.min+i*this.stepValue).toFixed(t)}));this.yLabelWidth=this.display&&this.showLabels?z(this.ctx,this.font,this.yLabels)+10:0},addXLabel:function(t){this.xLabels.push(t),this.valuesCount++,this.fit()},removeXLabel:function(){this.xLabels.shift(),this.valuesCount--,this.fit()},fit:function(){this.startPoint=this.display?this.fontSize:0,this.endPoint=this.display?this.height-1.5*this.fontSize-5:this.height,this.startPoint+=this.padding,this.endPoint-=this.padding;var t,i=this.endPoint,e=this.endPoint-this.startPoint;for(this.calculateYRange(e),this.buildYLabels(),this.calculateXLabelRotation();e>this.endPoint-this.startPoint;)e=this.endPoint-this.startPoint,t=this.yLabelWidth,this.calculateYRange(e),this.buildYLabels(),t<this.yLabelWidth&&(this.endPoint=i,this.calculateXLabelRotation())},calculateXLabelRotation:function(){this.ctx.font=this.font;var t,i,e=this.ctx.measureText(this.xLabels[0]).width,s=this.ctx.measureText(this.xLabels[this.xLabels.length-1]).width;if(this.xScalePaddingRight=s/2+3,this.xScalePaddingLeft=e/2>this.yLabelWidth?e/2:this.yLabelWidth,this.xLabelRotation=0,this.display){var n,o=z(this.ctx,this.font,this.xLabels);this.xLabelWidth=o;for(var a=Math.floor(this.calculateX(1)-this.calculateX(0))-6;this.xLabelWidth>a&&0===this.xLabelRotation||this.xLabelWidth>a&&this.xLabelRotation<=90&&this.xLabelRotation>0;)n=Math.cos(S(this.xLabelRotation)),t=n*e,i=n*s,t+this.fontSize/2>this.yLabelWidth&&(this.xScalePaddingLeft=t+this.fontSize/2),this.xScalePaddingRight=this.fontSize/2,this.xLabelRotation++,this.xLabelWidth=n*o;this.xLabelRotation>0&&(this.endPoint-=Math.sin(S(this.xLabelRotation))*o+3)}else this.xLabelWidth=0,this.xScalePaddingRight=this.padding,this.xScalePaddingLeft=this.padding},calculateYRange:c,drawingArea:function(){return this.startPoint-this.endPoint},calculateY:function(t){var i=this.drawingArea()/(this.min-this.max);return this.endPoint-i*(t-this.min)},calculateX:function(t){var i=(this.xLabelRotation>0,this.width-(this.xScalePaddingLeft+this.xScalePaddingRight)),e=i/Math.max(this.valuesCount-(this.offsetGridLines?0:1),1),s=e*t+this.xScalePaddingLeft;return this.offsetGridLines&&(s+=e/2),Math.round(s)},update:function(t){s.extend(this,t),this.fit()},draw:function(){var t=this.ctx,i=(this.endPoint-this.startPoint)/this.steps,e=Math.round(this.xScalePaddingLeft);this.display&&(t.fillStyle=this.textColor,t.font=this.font,n(this.yLabels,function(n,o){var a=this.endPoint-i*o,h=Math.round(a),l=this.showHorizontalLines;t.textAlign="right",t.textBaseline="middle",this.showLabels&&t.fillText(n,e-10,a),0!==o||l||(l=!0),l&&t.beginPath(),o>0?(t.lineWidth=this.gridLineWidth,t.strokeStyle=this.gridLineColor):(t.lineWidth=this.lineWidth,t.strokeStyle=this.lineColor),h+=s.aliasPixel(t.lineWidth),l&&(t.moveTo(e,h),t.lineTo(this.width,h),t.stroke(),t.closePath()),t.lineWidth=this.lineWidth,t.strokeStyle=this.lineColor,t.beginPath(),t.moveTo(e-5,h),t.lineTo(e,h),t.stroke(),t.closePath()},this),n(this.xLabels,function(i,e){var s=this.calculateX(e)+C(this.lineWidth),n=this.calculateX(e-(this.offsetGridLines?.5:0))+C(this.lineWidth),o=this.xLabelRotation>0,a=this.showVerticalLines;0!==e||a||(a=!0),a&&t.beginPath(),e>0?(t.lineWidth=this.gridLineWidth,t.strokeStyle=this.gridLineColor):(t.lineWidth=this.lineWidth,t.strokeStyle=this.lineColor),a&&(t.moveTo(n,this.endPoint),t.lineTo(n,this.startPoint-3),t.stroke(),t.closePath()),t.lineWidth=this.lineWidth,t.strokeStyle=this.lineColor,t.beginPath(),t.moveTo(n,this.endPoint),t.lineTo(n,this.endPoint+5),t.stroke(),t.closePath(),t.save(),t.translate(s,o?this.endPoint+12:this.endPoint+8),t.rotate(-1*S(this.xLabelRotation)),t.font=this.font,t.textAlign=o?"right":"center",t.textBaseline=o?"middle":"top",t.fillText(i,0,0),t.restore()},this))}}),e.RadialScale=e.Element.extend({initialize:function(){this.size=m([this.height,this.width]),this.drawingArea=this.display?this.size/2-(this.fontSize/2+this.backdropPaddingY):this.size/2},calculateCenterOffset:function(t){var i=this.drawingArea/(this.max-this.min);return(t-this.min)*i},update:function(){this.lineArc?this.drawingArea=this.display?this.size/2-(this.fontSize/2+this.backdropPaddingY):this.size/2:this.setScaleSize(),this.buildYLabels()},buildYLabels:function(){this.yLabels=[];for(var t=v(this.stepValue),i=0;i<=this.steps;i++)this.yLabels.push(y(this.templateString,{value:(this.min+i*this.stepValue).toFixed(t)}))},getCircumference:function(){return 2*Math.PI/this.valuesCount},setScaleSize:function(){var t,i,e,s,n,o,a,h,l,r,c,u,d=m([this.height/2-this.pointLabelFontSize-5,this.width/2]),p=this.width,g=0;for(this.ctx.font=W(this.pointLabelFontSize,this.pointLabelFontStyle,this.pointLabelFontFamily),i=0;i<this.valuesCount;i++)t=this.getPointPosition(i,d),e=this.ctx.measureText(y(this.templateString,{value:this.labels[i]})).width+5,0===i||i===this.valuesCount/2?(s=e/2,t.x+s>p&&(p=t.x+s,n=i),t.x-s<g&&(g=t.x-s,a=i)):i<this.valuesCount/2?t.x+e>p&&(p=t.x+e,n=i):i>this.valuesCount/2&&t.x-e<g&&(g=t.x-e,a=i);l=g,r=Math.ceil(p-this.width),o=this.getIndexAngle(n),h=this.getIndexAngle(a),c=r/Math.sin(o+Math.PI/2),u=l/Math.sin(h+Math.PI/2),c=f(c)?c:0,u=f(u)?u:0,this.drawingArea=d-(u+c)/2,this.setCenterPoint(u,c)},setCenterPoint:function(t,i){var e=this.width-i-this.drawingArea,s=t+this.drawingArea;this.xCenter=(s+e)/2,this.yCenter=this.height/2},getIndexAngle:function(t){var i=2*Math.PI/this.valuesCount;return t*i-Math.PI/2},getPointPosition:function(t,i){var e=this.getIndexAngle(t);return{x:Math.cos(e)*i+this.xCenter,y:Math.sin(e)*i+this.yCenter}},draw:function(){if(this.display){var t=this.ctx;if(n(this.yLabels,function(i,e){if(e>0){var s,n=e*(this.drawingArea/this.steps),o=this.yCenter-n;if(this.lineWidth>0)if(t.strokeStyle=this.lineColor,t.lineWidth=this.lineWidth,this.lineArc)t.beginPath(),t.arc(this.xCenter,this.yCenter,n,0,2*Math.PI),t.closePath(),t.stroke();else{t.beginPath();for(var a=0;a<this.valuesCount;a++)s=this.getPointPosition(a,this.calculateCenterOffset(this.min+e*this.stepValue)),0===a?t.moveTo(s.x,s.y):t.lineTo(s.x,s.y);t.closePath(),t.stroke()}if(this.showLabels){if(t.font=W(this.fontSize,this.fontStyle,this.fontFamily),this.showLabelBackdrop){var h=t.measureText(i).width;t.fillStyle=this.backdropColor,t.fillRect(this.xCenter-h/2-this.backdropPaddingX,o-this.fontSize/2-this.backdropPaddingY,h+2*this.backdropPaddingX,this.fontSize+2*this.backdropPaddingY)}t.textAlign="center",t.textBaseline="middle",t.fillStyle=this.fontColor,t.fillText(i,this.xCenter,o)}}},this),!this.lineArc){t.lineWidth=this.angleLineWidth,t.strokeStyle=this.angleLineColor;for(var i=this.valuesCount-1;i>=0;i--){var e=null,s=null;if(this.angleLineWidth>0&&i%this.angleLineInterval===0&&(e=this.calculateCenterOffset(this.max),s=this.getPointPosition(i,e),t.beginPath(),t.moveTo(this.xCenter,this.yCenter),t.lineTo(s.x,s.y),t.stroke(),t.closePath()),this.backgroundColors&&this.backgroundColors.length==this.valuesCount){null==e&&(e=this.calculateCenterOffset(this.max)),null==s&&(s=this.getPointPosition(i,e));var o=this.getPointPosition(0===i?this.valuesCount-1:i-1,e),a=this.getPointPosition(i===this.valuesCount-1?0:i+1,e),h={x:(o.x+s.x)/2,y:(o.y+s.y)/2},l={x:(s.x+a.x)/2,y:(s.y+a.y)/2};t.beginPath(),t.moveTo(this.xCenter,this.yCenter),t.lineTo(h.x,h.y),t.lineTo(s.x,s.y),t.lineTo(l.x,l.y),t.fillStyle=this.backgroundColors[i],t.fill(),t.closePath()}var r=this.getPointPosition(i,this.calculateCenterOffset(this.max)+5);t.font=W(this.pointLabelFontSize,this.pointLabelFontStyle,this.pointLabelFontFamily),t.fillStyle=this.pointLabelFontColor;var c=this.labels.length,u=this.labels.length/2,d=u/2,p=d>i||i>c-d,f=i===d||i===c-d;0===i?t.textAlign="center":i===u?t.textAlign="center":u>i?t.textAlign="left":t.textAlign="right",f?t.textBaseline="middle":p?t.textBaseline="bottom":t.textBaseline="top",t.fillText(this.labels[i],r.x,r.y)}}}}}),e.animationService={frameDuration:17,animations:[],dropFrames:0,addAnimation:function(t,i){for(var e=0;e<this.animations.length;++e)if(this.animations[e].chartInstance===t)return void(this.animations[e].animationObject=i);this.animations.push({chartInstance:t,animationObject:i}),1==this.animations.length&&s.requestAnimFrame.call(window,this.digestWrapper)},cancelAnimation:function(t){var i=s.findNextWhere(this.animations,function(i){return i.chartInstance===t});i&&this.animations.splice(i,1)},digestWrapper:function(){e.animationService.startDigest.call(e.animationService)},startDigest:function(){var t=Date.now(),i=0;this.dropFrames>1&&(i=Math.floor(this.dropFrames),this.dropFrames-=i);for(var e=0;e<this.animations.length;e++)null===this.animations[e].animationObject.currentStep&&(this.animations[e].animationObject.currentStep=0),this.animations[e].animationObject.currentStep+=1+i,this.animations[e].animationObject.currentStep>this.animations[e].animationObject.numSteps&&(this.animations[e].animationObject.currentStep=this.animations[e].animationObject.numSteps),this.animations[e].animationObject.render(this.animations[e].chartInstance,this.animations[e].animationObject),this.animations[e].animationObject.currentStep==this.animations[e].animationObject.numSteps&&(this.animations[e].animationObject.onAnimationComplete.call(this.animations[e].chartInstance),this.animations.splice(e,1),e--);var n=Date.now(),o=n-t-this.frameDuration,a=o/this.frameDuration;a>1&&(this.dropFrames+=a),this.animations.length>0&&s.requestAnimFrame.call(window,this.digestWrapper)}},s.addEvent(window,"resize",function(){var t;return function(){clearTimeout(t),t=setTimeout(function(){n(e.instances,function(t){t.options.responsive&&t.resize(t.render,!0)})},50)}}()),p?define("Chart",[],function(){return e}):"object"==typeof module&&module.exports&&(module.exports=e),t.Chart=e,e.noConflict=function(){return t.Chart=i,e}}).call(this),function(){"use strict";var t=this,i=t.Chart,e=i.helpers,s={scaleBeginAtZero:!0,scaleShowGridLines:!0,scaleGridLineColor:"rgba(0,0,0,.05)",scaleGridLineWidth:1,scaleShowHorizontalLines:!0,scaleShowVerticalLines:!0,barShowStroke:!0,barStrokeWidth:2,barValueSpacing:5,barDatasetSpacing:1,legendTemplate:'<ul class="<%=name.toLowerCase()%>-legend"><% for (var i=0; i<datasets.length; i++){%><li><span class="<%=name.toLowerCase()%>-legend-icon" style="background-color:<%=datasets[i].fillColor%>"></span><span class="<%=name.toLowerCase()%>-legend-text"><%if(datasets[i].label){%><%=datasets[i].label%><%}%></span></li><%}%></ul>'};i.Type.extend({name:"Bar",defaults:s,initialize:function(t){var s=this.options;this.ScaleClass=i.Scale.extend({offsetGridLines:!0,calculateBarX:function(t,i,e){var n=this.calculateBaseWidth(),o=this.calculateX(e)-n/2,a=this.calculateBarWidth(t);return o+a*i+i*s.barDatasetSpacing+a/2},calculateBaseWidth:function(){return this.calculateX(1)-this.calculateX(0)-2*s.barValueSpacing;
},calculateBarWidth:function(t){var i=this.calculateBaseWidth()-(t-1)*s.barDatasetSpacing;return i/t}}),this.datasets=[],this.options.showTooltips&&e.bindEvents(this,this.options.tooltipEvents,function(t){var i="mouseout"!==t.type?this.getBarsAtEvent(t):[];this.eachBars(function(t){t.restore(["fillColor","strokeColor"])}),e.each(i,function(t){t&&(t.fillColor=t.highlightFill,t.strokeColor=t.highlightStroke)}),this.showTooltip(i)}),this.BarClass=i.Rectangle.extend({strokeWidth:this.options.barStrokeWidth,showStroke:this.options.barShowStroke,ctx:this.chart.ctx}),e.each(t.datasets,function(i,s){var n={label:i.label||null,fillColor:i.fillColor,strokeColor:i.strokeColor,bars:[]};this.datasets.push(n),e.each(i.data,function(e,s){n.bars.push(new this.BarClass({value:e,label:t.labels[s],datasetLabel:i.label,strokeColor:"object"==typeof i.strokeColor?i.strokeColor[s]:i.strokeColor,fillColor:"object"==typeof i.fillColor?i.fillColor[s]:i.fillColor,highlightFill:i.highlightFill?"object"==typeof i.highlightFill?i.highlightFill[s]:i.highlightFill:"object"==typeof i.fillColor?i.fillColor[s]:i.fillColor,highlightStroke:i.highlightStroke?"object"==typeof i.highlightStroke?i.highlightStroke[s]:i.highlightStroke:"object"==typeof i.strokeColor?i.strokeColor[s]:i.strokeColor}))},this)},this),this.buildScale(t.labels),this.BarClass.prototype.base=this.scale.endPoint,this.eachBars(function(t,i,s){e.extend(t,{width:this.scale.calculateBarWidth(this.datasets.length),x:this.scale.calculateBarX(this.datasets.length,s,i),y:this.scale.endPoint}),t.save()},this),this.render()},update:function(){this.scale.update(),e.each(this.activeElements,function(t){t.restore(["fillColor","strokeColor"])}),this.eachBars(function(t){t.save()}),this.render()},eachBars:function(t){e.each(this.datasets,function(i,s){e.each(i.bars,t,this,s)},this)},getBarsAtEvent:function(t){for(var i,s=[],n=e.getRelativePosition(t),o=function(t){s.push(t.bars[i])},a=0;a<this.datasets.length;a++)for(i=0;i<this.datasets[a].bars.length;i++)if(this.datasets[a].bars[i].inRange(n.x,n.y))return e.each(this.datasets,o),s;return s},buildScale:function(t){var i=this,s=function(){var t=[];return i.eachBars(function(i){t.push(i.value)}),t},n={templateString:this.options.scaleLabel,height:this.chart.height,width:this.chart.width,ctx:this.chart.ctx,textColor:this.options.scaleFontColor,fontSize:this.options.scaleFontSize,fontStyle:this.options.scaleFontStyle,fontFamily:this.options.scaleFontFamily,valuesCount:t.length,beginAtZero:this.options.scaleBeginAtZero,integersOnly:this.options.scaleIntegersOnly,calculateYRange:function(t){var i=e.calculateScaleRange(s(),t,this.fontSize,this.beginAtZero,this.integersOnly);e.extend(this,i)},xLabels:t,font:e.fontString(this.options.scaleFontSize,this.options.scaleFontStyle,this.options.scaleFontFamily),lineWidth:this.options.scaleLineWidth,lineColor:this.options.scaleLineColor,showHorizontalLines:this.options.scaleShowHorizontalLines,showVerticalLines:this.options.scaleShowVerticalLines,gridLineWidth:this.options.scaleShowGridLines?this.options.scaleGridLineWidth:0,gridLineColor:this.options.scaleShowGridLines?this.options.scaleGridLineColor:"rgba(0,0,0,0)",padding:this.options.showScale?0:this.options.barShowStroke?this.options.barStrokeWidth:0,showLabels:this.options.scaleShowLabels,display:this.options.showScale};this.options.scaleOverride&&e.extend(n,{calculateYRange:e.noop,steps:this.options.scaleSteps,stepValue:this.options.scaleStepWidth,min:this.options.scaleStartValue,max:this.options.scaleStartValue+this.options.scaleSteps*this.options.scaleStepWidth}),this.scale=new this.ScaleClass(n)},addData:function(t,i){e.each(t,function(t,e){this.datasets[e].bars.push(new this.BarClass({value:t,label:i,datasetLabel:this.datasets[e].label,x:this.scale.calculateBarX(this.datasets.length,e,this.scale.valuesCount+1),y:this.scale.endPoint,width:this.scale.calculateBarWidth(this.datasets.length),base:this.scale.endPoint,strokeColor:this.datasets[e].strokeColor,fillColor:this.datasets[e].fillColor}))},this),this.scale.addXLabel(i),this.update()},removeData:function(){this.scale.removeXLabel(),e.each(this.datasets,function(t){t.bars.shift()},this),this.update()},reflow:function(){e.extend(this.BarClass.prototype,{y:this.scale.endPoint,base:this.scale.endPoint});var t=e.extend({height:this.chart.height,width:this.chart.width});this.scale.update(t)},draw:function(t){var i=t||1;this.clear();this.chart.ctx;this.scale.draw(i),e.each(this.datasets,function(t,s){e.each(t.bars,function(t,e){t.hasValue()&&(t.base=this.scale.endPoint,t.transition({x:this.scale.calculateBarX(this.datasets.length,s,e),y:this.scale.calculateY(t.value),width:this.scale.calculateBarWidth(this.datasets.length)},i).draw())},this)},this)}})}.call(this),function(){"use strict";var t=this,i=t.Chart,e=i.helpers,s={segmentShowStroke:!0,segmentStrokeColor:"#fff",segmentStrokeWidth:2,percentageInnerCutout:50,animationSteps:100,animationEasing:"easeOutBounce",animateRotate:!0,animateScale:!1,legendTemplate:'<ul class="<%=name.toLowerCase()%>-legend"><% for (var i=0; i<segments.length; i++){%><li><span class="<%=name.toLowerCase()%>-legend-icon" style="background-color:<%=segments[i].fillColor%>"></span><span class="<%=name.toLowerCase()%>-legend-text"><%if(segments[i].label){%><%=segments[i].label%><%}%></span></li><%}%></ul>'};i.Type.extend({name:"Doughnut",defaults:s,initialize:function(t){this.segments=[],this.outerRadius=(e.min([this.chart.width,this.chart.height])-this.options.segmentStrokeWidth/2)/2,this.SegmentArc=i.Arc.extend({ctx:this.chart.ctx,x:this.chart.width/2,y:this.chart.height/2}),this.options.showTooltips&&e.bindEvents(this,this.options.tooltipEvents,function(t){var i="mouseout"!==t.type?this.getSegmentsAtEvent(t):[];e.each(this.segments,function(t){t.restore(["fillColor"])}),e.each(i,function(t){t.fillColor=t.highlightColor}),this.showTooltip(i)}),this.calculateTotal(t),e.each(t,function(i,e){i.color||(i.color="hsl("+360*e/t.length+", 100%, 50%)"),this.addData(i,e,!0)},this),this.render()},getSegmentsAtEvent:function(t){var i=[],s=e.getRelativePosition(t);return e.each(this.segments,function(t){t.inRange(s.x,s.y)&&i.push(t)},this),i},addData:function(t,e,s){var n=void 0!==e?e:this.segments.length;"undefined"==typeof t.color&&(t.color=i.defaults.global.segmentColorDefault[n%i.defaults.global.segmentColorDefault.length],t.highlight=i.defaults.global.segmentHighlightColorDefaults[n%i.defaults.global.segmentHighlightColorDefaults.length]),this.segments.splice(n,0,new this.SegmentArc({value:t.value,outerRadius:this.options.animateScale?0:this.outerRadius,innerRadius:this.options.animateScale?0:this.outerRadius/100*this.options.percentageInnerCutout,fillColor:t.color,highlightColor:t.highlight||t.color,showStroke:this.options.segmentShowStroke,strokeWidth:this.options.segmentStrokeWidth,strokeColor:this.options.segmentStrokeColor,startAngle:1.5*Math.PI,circumference:this.options.animateRotate?0:this.calculateCircumference(t.value),label:t.label})),s||(this.reflow(),this.update())},calculateCircumference:function(t){return this.total>0?2*Math.PI*(t/this.total):0},calculateTotal:function(t){this.total=0,e.each(t,function(t){this.total+=Math.abs(t.value)},this)},update:function(){this.calculateTotal(this.segments),e.each(this.activeElements,function(t){t.restore(["fillColor"])}),e.each(this.segments,function(t){t.save()}),this.render()},removeData:function(t){var i=e.isNumber(t)?t:this.segments.length-1;this.segments.splice(i,1),this.reflow(),this.update()},reflow:function(){e.extend(this.SegmentArc.prototype,{x:this.chart.width/2,y:this.chart.height/2}),this.outerRadius=(e.min([this.chart.width,this.chart.height])-this.options.segmentStrokeWidth/2)/2,e.each(this.segments,function(t){t.update({outerRadius:this.outerRadius,innerRadius:this.outerRadius/100*this.options.percentageInnerCutout})},this)},draw:function(t){var i=t?t:1;this.clear(),e.each(this.segments,function(t,e){t.transition({circumference:this.calculateCircumference(t.value),outerRadius:this.outerRadius,innerRadius:this.outerRadius/100*this.options.percentageInnerCutout},i),t.endAngle=t.startAngle+t.circumference,t.draw(),0===e&&(t.startAngle=1.5*Math.PI),e<this.segments.length-1&&(this.segments[e+1].startAngle=t.endAngle)},this)}}),i.types.Doughnut.extend({name:"Pie",defaults:e.merge(s,{percentageInnerCutout:0})})}.call(this),function(){"use strict";var t=this,i=t.Chart,e=i.helpers,s={scaleShowGridLines:!0,scaleGridLineColor:"rgba(0,0,0,.05)",scaleGridLineWidth:1,scaleShowHorizontalLines:!0,scaleShowVerticalLines:!0,bezierCurve:!0,bezierCurveTension:.4,pointDot:!0,pointDotRadius:4,pointDotStrokeWidth:1,pointHitDetectionRadius:20,datasetStroke:!0,datasetStrokeWidth:2,datasetFill:!0,legendTemplate:'<ul class="<%=name.toLowerCase()%>-legend"><% for (var i=0; i<datasets.length; i++){%><li><span class="<%=name.toLowerCase()%>-legend-icon" style="background-color:<%=datasets[i].strokeColor%>"></span><span class="<%=name.toLowerCase()%>-legend-text"><%if(datasets[i].label){%><%=datasets[i].label%><%}%></span></li><%}%></ul>',offsetGridLines:!1};i.Type.extend({name:"Line",defaults:s,initialize:function(t){this.PointClass=i.Point.extend({offsetGridLines:this.options.offsetGridLines,strokeWidth:this.options.pointDotStrokeWidth,radius:this.options.pointDotRadius,display:this.options.pointDot,hitDetectionRadius:this.options.pointHitDetectionRadius,ctx:this.chart.ctx,inRange:function(t){return Math.pow(t-this.x,2)<Math.pow(this.radius+this.hitDetectionRadius,2)}}),this.datasets=[],this.options.showTooltips&&e.bindEvents(this,this.options.tooltipEvents,function(t){var i="mouseout"!==t.type?this.getPointsAtEvent(t):[];this.eachPoints(function(t){t.restore(["fillColor","strokeColor"])}),e.each(i,function(t){t.fillColor=t.highlightFill,t.strokeColor=t.highlightStroke}),this.showTooltip(i)}),e.each(t.datasets,function(i){var s={label:i.label||null,fillColor:i.fillColor,strokeColor:i.strokeColor,pointColor:i.pointColor,pointStrokeColor:i.pointStrokeColor,points:[]};this.datasets.push(s),e.each(i.data,function(e,n){s.points.push(new this.PointClass({value:e,label:t.labels[n],datasetLabel:i.label,strokeColor:i.pointStrokeColor,fillColor:i.pointColor,highlightFill:i.pointHighlightFill||i.pointColor,highlightStroke:i.pointHighlightStroke||i.pointStrokeColor}))},this),this.buildScale(t.labels),this.eachPoints(function(t,i){e.extend(t,{x:this.scale.calculateX(i),y:this.scale.endPoint}),t.save()},this)},this),this.render()},update:function(){this.scale.update(),e.each(this.activeElements,function(t){t.restore(["fillColor","strokeColor"])}),this.eachPoints(function(t){t.save()}),this.render()},eachPoints:function(t){e.each(this.datasets,function(i){e.each(i.points,t,this)},this)},getPointsAtEvent:function(t){var i=[],s=e.getRelativePosition(t);return e.each(this.datasets,function(t){e.each(t.points,function(t){t.inRange(s.x,s.y)&&i.push(t)})},this),i},buildScale:function(t){var s=this,n=function(){var t=[];return s.eachPoints(function(i){t.push(i.value)}),t},o={templateString:this.options.scaleLabel,height:this.chart.height,width:this.chart.width,ctx:this.chart.ctx,textColor:this.options.scaleFontColor,offsetGridLines:this.options.offsetGridLines,fontSize:this.options.scaleFontSize,fontStyle:this.options.scaleFontStyle,fontFamily:this.options.scaleFontFamily,valuesCount:t.length,beginAtZero:this.options.scaleBeginAtZero,integersOnly:this.options.scaleIntegersOnly,calculateYRange:function(t){var i=e.calculateScaleRange(n(),t,this.fontSize,this.beginAtZero,this.integersOnly);e.extend(this,i)},xLabels:t,font:e.fontString(this.options.scaleFontSize,this.options.scaleFontStyle,this.options.scaleFontFamily),lineWidth:this.options.scaleLineWidth,lineColor:this.options.scaleLineColor,showHorizontalLines:this.options.scaleShowHorizontalLines,showVerticalLines:this.options.scaleShowVerticalLines,gridLineWidth:this.options.scaleShowGridLines?this.options.scaleGridLineWidth:0,gridLineColor:this.options.scaleShowGridLines?this.options.scaleGridLineColor:"rgba(0,0,0,0)",padding:this.options.showScale?0:this.options.pointDotRadius+this.options.pointDotStrokeWidth,showLabels:this.options.scaleShowLabels,display:this.options.showScale};this.options.scaleOverride&&e.extend(o,{calculateYRange:e.noop,steps:this.options.scaleSteps,stepValue:this.options.scaleStepWidth,min:this.options.scaleStartValue,max:this.options.scaleStartValue+this.options.scaleSteps*this.options.scaleStepWidth}),this.scale=new i.Scale(o)},addData:function(t,i){e.each(t,function(t,e){this.datasets[e].points.push(new this.PointClass({value:t,label:i,datasetLabel:this.datasets[e].label,x:this.scale.calculateX(this.scale.valuesCount+1),y:this.scale.endPoint,strokeColor:this.datasets[e].pointStrokeColor,fillColor:this.datasets[e].pointColor}))},this),this.scale.addXLabel(i),this.update()},removeData:function(){this.scale.removeXLabel(),e.each(this.datasets,function(t){t.points.shift()},this),this.update()},reflow:function(){var t=e.extend({height:this.chart.height,width:this.chart.width});this.scale.update(t)},draw:function(t){var i=t||1;this.clear();var s=this.chart.ctx,n=function(t){return null!==t.value},o=function(t,i,s){return e.findNextWhere(i,n,s)||t},a=function(t,i,s){return e.findPreviousWhere(i,n,s)||t};this.scale&&(this.scale.draw(i),e.each(this.datasets,function(t){var h=e.where(t.points,n);e.each(t.points,function(t,e){t.hasValue()&&t.transition({y:this.scale.calculateY(t.value),x:this.scale.calculateX(e)},i)},this),this.options.bezierCurve&&e.each(h,function(t,i){var s=i>0&&i<h.length-1?this.options.bezierCurveTension:0;t.controlPoints=e.splineCurve(a(t,h,i),t,o(t,h,i),s),t.controlPoints.outer.y>this.scale.endPoint?t.controlPoints.outer.y=this.scale.endPoint:t.controlPoints.outer.y<this.scale.startPoint&&(t.controlPoints.outer.y=this.scale.startPoint),t.controlPoints.inner.y>this.scale.endPoint?t.controlPoints.inner.y=this.scale.endPoint:t.controlPoints.inner.y<this.scale.startPoint&&(t.controlPoints.inner.y=this.scale.startPoint)},this),s.lineWidth=this.options.datasetStrokeWidth,s.strokeStyle=t.strokeColor,s.beginPath(),e.each(h,function(t,i){if(0===i)s.moveTo(t.x,t.y);else if(this.options.bezierCurve){var e=a(t,h,i);s.bezierCurveTo(e.controlPoints.outer.x,e.controlPoints.outer.y,t.controlPoints.inner.x,t.controlPoints.inner.y,t.x,t.y)}else s.lineTo(t.x,t.y)},this),this.options.datasetStroke&&s.stroke(),this.options.datasetFill&&h.length>0&&(s.lineTo(h[h.length-1].x,this.scale.endPoint),s.lineTo(h[0].x,this.scale.endPoint),s.fillStyle=t.fillColor,s.closePath(),s.fill()),e.each(h,function(t){t.draw()})},this))}})}.call(this),function(){"use strict";var t=this,i=t.Chart,e=i.helpers,s={scaleShowLabelBackdrop:!0,scaleBackdropColor:"rgba(255,255,255,0.75)",scaleBeginAtZero:!0,scaleBackdropPaddingY:2,scaleBackdropPaddingX:2,scaleShowLine:!0,segmentShowStroke:!0,segmentStrokeColor:"#fff",segmentStrokeWidth:2,animationSteps:100,animationEasing:"easeOutBounce",animateRotate:!0,animateScale:!1,legendTemplate:'<ul class="<%=name.toLowerCase()%>-legend"><% for (var i=0; i<segments.length; i++){%><li><span class="<%=name.toLowerCase()%>-legend-icon" style="background-color:<%=segments[i].fillColor%>"></span><span class="<%=name.toLowerCase()%>-legend-text"><%if(segments[i].label){%><%=segments[i].label%><%}%></span></li><%}%></ul>'};i.Type.extend({name:"PolarArea",defaults:s,initialize:function(t){this.segments=[],this.SegmentArc=i.Arc.extend({showStroke:this.options.segmentShowStroke,strokeWidth:this.options.segmentStrokeWidth,strokeColor:this.options.segmentStrokeColor,ctx:this.chart.ctx,innerRadius:0,x:this.chart.width/2,y:this.chart.height/2}),this.scale=new i.RadialScale({display:this.options.showScale,fontStyle:this.options.scaleFontStyle,fontSize:this.options.scaleFontSize,fontFamily:this.options.scaleFontFamily,fontColor:this.options.scaleFontColor,showLabels:this.options.scaleShowLabels,showLabelBackdrop:this.options.scaleShowLabelBackdrop,backdropColor:this.options.scaleBackdropColor,backdropPaddingY:this.options.scaleBackdropPaddingY,backdropPaddingX:this.options.scaleBackdropPaddingX,lineWidth:this.options.scaleShowLine?this.options.scaleLineWidth:0,lineColor:this.options.scaleLineColor,lineArc:!0,width:this.chart.width,height:this.chart.height,xCenter:this.chart.width/2,yCenter:this.chart.height/2,ctx:this.chart.ctx,templateString:this.options.scaleLabel,valuesCount:t.length}),this.updateScaleRange(t),this.scale.update(),e.each(t,function(t,i){this.addData(t,i,!0)},this),this.options.showTooltips&&e.bindEvents(this,this.options.tooltipEvents,function(t){var i="mouseout"!==t.type?this.getSegmentsAtEvent(t):[];e.each(this.segments,function(t){t.restore(["fillColor"])}),e.each(i,function(t){t.fillColor=t.highlightColor}),this.showTooltip(i)}),this.render()},getSegmentsAtEvent:function(t){var i=[],s=e.getRelativePosition(t);return e.each(this.segments,function(t){t.inRange(s.x,s.y)&&i.push(t)},this),i},addData:function(t,i,e){var s=i||this.segments.length;this.segments.splice(s,0,new this.SegmentArc({fillColor:t.color,highlightColor:t.highlight||t.color,label:t.label,value:t.value,outerRadius:this.options.animateScale?0:this.scale.calculateCenterOffset(t.value),circumference:this.options.animateRotate?0:this.scale.getCircumference(),startAngle:1.5*Math.PI})),e||(this.reflow(),this.update())},removeData:function(t){var i=e.isNumber(t)?t:this.segments.length-1;this.segments.splice(i,1),this.reflow(),this.update()},calculateTotal:function(t){this.total=0,e.each(t,function(t){this.total+=t.value},this),this.scale.valuesCount=this.segments.length},updateScaleRange:function(t){var i=[];e.each(t,function(t){i.push(t.value)});var s=this.options.scaleOverride?{steps:this.options.scaleSteps,stepValue:this.options.scaleStepWidth,min:this.options.scaleStartValue,max:this.options.scaleStartValue+this.options.scaleSteps*this.options.scaleStepWidth}:e.calculateScaleRange(i,e.min([this.chart.width,this.chart.height])/2,this.options.scaleFontSize,this.options.scaleBeginAtZero,this.options.scaleIntegersOnly);e.extend(this.scale,s,{size:e.min([this.chart.width,this.chart.height]),xCenter:this.chart.width/2,yCenter:this.chart.height/2})},update:function(){this.calculateTotal(this.segments),e.each(this.segments,function(t){t.save()}),this.reflow(),this.render()},reflow:function(){e.extend(this.SegmentArc.prototype,{x:this.chart.width/2,y:this.chart.height/2}),this.updateScaleRange(this.segments),this.scale.update(),e.extend(this.scale,{xCenter:this.chart.width/2,yCenter:this.chart.height/2}),e.each(this.segments,function(t){t.update({outerRadius:this.scale.calculateCenterOffset(t.value)})},this)},draw:function(t){var i=t||1;this.clear(),e.each(this.segments,function(t,e){t.transition({circumference:this.scale.getCircumference(),outerRadius:this.scale.calculateCenterOffset(t.value)},i),t.endAngle=t.startAngle+t.circumference,0===e&&(t.startAngle=1.5*Math.PI),e<this.segments.length-1&&(this.segments[e+1].startAngle=t.endAngle),t.draw()},this),this.scale.draw()}})}.call(this),function(){"use strict";var t=this,i=t.Chart,e=i.helpers;i.Type.extend({name:"Radar",defaults:{scaleShowLine:!0,angleShowLineOut:!0,scaleShowLabels:!1,scaleBeginAtZero:!0,angleLineColor:"rgba(0,0,0,.1)",angleLineWidth:1,angleLineInterval:1,pointLabelFontFamily:"'Arial'",pointLabelFontStyle:"normal",pointLabelFontSize:10,pointLabelFontColor:"#666",pointDot:!0,pointDotRadius:3,pointDotStrokeWidth:1,pointHitDetectionRadius:20,datasetStroke:!0,datasetStrokeWidth:2,datasetFill:!0,legendTemplate:'<ul class="<%=name.toLowerCase()%>-legend"><% for (var i=0; i<datasets.length; i++){%><li><span class="<%=name.toLowerCase()%>-legend-icon" style="background-color:<%=datasets[i].strokeColor%>"></span><span class="<%=name.toLowerCase()%>-legend-text"><%if(datasets[i].label){%><%=datasets[i].label%><%}%></span></li><%}%></ul>'},initialize:function(t){this.PointClass=i.Point.extend({strokeWidth:this.options.pointDotStrokeWidth,radius:this.options.pointDotRadius,display:this.options.pointDot,hitDetectionRadius:this.options.pointHitDetectionRadius,ctx:this.chart.ctx}),this.datasets=[],this.buildScale(t),this.options.showTooltips&&e.bindEvents(this,this.options.tooltipEvents,function(t){var i="mouseout"!==t.type?this.getPointsAtEvent(t):[];this.eachPoints(function(t){t.restore(["fillColor","strokeColor"])}),e.each(i,function(t){t.fillColor=t.highlightFill,t.strokeColor=t.highlightStroke}),this.showTooltip(i)}),e.each(t.datasets,function(i){var s={label:i.label||null,fillColor:i.fillColor,strokeColor:i.strokeColor,pointColor:i.pointColor,pointStrokeColor:i.pointStrokeColor,points:[]};this.datasets.push(s),e.each(i.data,function(e,n){var o;this.scale.animation||(o=this.scale.getPointPosition(n,this.scale.calculateCenterOffset(e))),s.points.push(new this.PointClass({value:e,label:t.labels[n],datasetLabel:i.label,x:this.options.animation?this.scale.xCenter:o.x,y:this.options.animation?this.scale.yCenter:o.y,strokeColor:i.pointStrokeColor,fillColor:i.pointColor,highlightFill:i.pointHighlightFill||i.pointColor,highlightStroke:i.pointHighlightStroke||i.pointStrokeColor}))},this)},this),this.render()},eachPoints:function(t){e.each(this.datasets,function(i){e.each(i.points,t,this)},this)},getPointsAtEvent:function(t){var i=e.getRelativePosition(t),s=e.getAngleFromPoint({x:this.scale.xCenter,y:this.scale.yCenter},i),n=2*Math.PI/this.scale.valuesCount,o=Math.round((s.angle-1.5*Math.PI)/n),a=[];return(o>=this.scale.valuesCount||0>o)&&(o=0),s.distance<=this.scale.drawingArea&&e.each(this.datasets,function(t){a.push(t.points[o])}),a},buildScale:function(t){this.scale=new i.RadialScale({display:this.options.showScale,fontStyle:this.options.scaleFontStyle,fontSize:this.options.scaleFontSize,fontFamily:this.options.scaleFontFamily,fontColor:this.options.scaleFontColor,showLabels:this.options.scaleShowLabels,showLabelBackdrop:this.options.scaleShowLabelBackdrop,backdropColor:this.options.scaleBackdropColor,backgroundColors:this.options.scaleBackgroundColors,backdropPaddingY:this.options.scaleBackdropPaddingY,backdropPaddingX:this.options.scaleBackdropPaddingX,lineWidth:this.options.scaleShowLine?this.options.scaleLineWidth:0,lineColor:this.options.scaleLineColor,angleLineColor:this.options.angleLineColor,angleLineWidth:this.options.angleShowLineOut?this.options.angleLineWidth:0,angleLineInterval:this.options.angleLineInterval?this.options.angleLineInterval:1,pointLabelFontColor:this.options.pointLabelFontColor,pointLabelFontSize:this.options.pointLabelFontSize,pointLabelFontFamily:this.options.pointLabelFontFamily,pointLabelFontStyle:this.options.pointLabelFontStyle,height:this.chart.height,width:this.chart.width,xCenter:this.chart.width/2,yCenter:this.chart.height/2,ctx:this.chart.ctx,templateString:this.options.scaleLabel,labels:t.labels,valuesCount:t.datasets[0].data.length}),this.scale.setScaleSize(),this.updateScaleRange(t.datasets),this.scale.buildYLabels()},updateScaleRange:function(t){var i=function(){var i=[];return e.each(t,function(t){t.data?i=i.concat(t.data):e.each(t.points,function(t){i.push(t.value)})}),i}(),s=this.options.scaleOverride?{steps:this.options.scaleSteps,stepValue:this.options.scaleStepWidth,min:this.options.scaleStartValue,max:this.options.scaleStartValue+this.options.scaleSteps*this.options.scaleStepWidth}:e.calculateScaleRange(i,e.min([this.chart.width,this.chart.height])/2,this.options.scaleFontSize,this.options.scaleBeginAtZero,this.options.scaleIntegersOnly);e.extend(this.scale,s)},addData:function(t,i){this.scale.valuesCount++,e.each(t,function(t,e){var s=this.scale.getPointPosition(this.scale.valuesCount,this.scale.calculateCenterOffset(t));this.datasets[e].points.push(new this.PointClass({value:t,label:i,datasetLabel:this.datasets[e].label,x:s.x,y:s.y,strokeColor:this.datasets[e].pointStrokeColor,fillColor:this.datasets[e].pointColor}))},this),this.scale.labels.push(i),this.reflow(),this.update()},removeData:function(){this.scale.valuesCount--,this.scale.labels.shift(),e.each(this.datasets,function(t){t.points.shift()},this),this.reflow(),this.update()},update:function(){this.eachPoints(function(t){t.save()}),this.reflow(),this.render()},reflow:function(){e.extend(this.scale,{width:this.chart.width,height:this.chart.height,size:e.min([this.chart.width,this.chart.height]),xCenter:this.chart.width/2,yCenter:this.chart.height/2}),this.updateScaleRange(this.datasets),this.scale.setScaleSize(),this.scale.buildYLabels()},draw:function(t){var i=t||1,s=this.chart.ctx;this.clear(),this.scale.draw(),e.each(this.datasets,function(t){e.each(t.points,function(t,e){t.hasValue()&&t.transition(this.scale.getPointPosition(e,this.scale.calculateCenterOffset(t.value)),i)},this),s.lineWidth=this.options.datasetStrokeWidth,s.strokeStyle=t.strokeColor,s.beginPath(),e.each(t.points,function(t,i){0===i?s.moveTo(t.x,t.y):s.lineTo(t.x,t.y)},this),s.closePath(),s.stroke(),s.fillStyle=t.fillColor,this.options.datasetFill&&s.fill(),e.each(t.points,function(t){t.hasValue()&&t.draw()})},this)}})}.call(this);

var step = 50;//
var readedData = [];
var ValName = "";
var iteration = 1;
var redy = 0;
var errorGet = 0;
var EndFuncAddr;

var lastYear = 0;
var lastMonth = 0;
var lastDay = 0;
var lastHour = 0;
var lastMinute = 0;
var lastSecond = 0;
var numberOfMeasProbes = 0;

var measureIntervall = 0;
//page cant be 0!
var page_number = 0;
var numOfProbesToRead = 100;

var fromProbeNumber;
var toProbeNumber;
var firstMeasureFromNumber;

function countFromTo(){
	fromProbeNumber = (((iteration-1)*step)+(page_number*(numOfProbesToRead)));
	toProbeNumber = (((iteration-1)*step+(page_number*numOfProbesToRead))+step);
	
}

function initMeasures(name,callEndFunc){
	iteration = 1;
	countFromTo();
	firstMeasureFromNumber = fromProbeNumber;
	ValName = name;
	getMeasureValues(name,fromProbeNumber,toProbeNumber,pushMeasure);
	errorGet = 0;
	readedData = [];

	EndFuncAddr = callEndFunc;
}
function pushMeasure(name,numberOfProbes,values,toProbNum){

	//countFromTo();

	if((((numOfProbesToRead * (page_number))+((iteration)*step))<(numOfProbesToRead*(page_number+1)))&(numberOfProbes>toProbNum)){
		for(var i = fromProbeNumber;i<toProbeNumber;i++){
			readedData.push(eval("values.V"+i.toString()));
		}
		iteration++;
		countFromTo();
		getMeasureValues(name,fromProbeNumber,toProbeNumber,pushMeasure);
		
		redy = 0;
	}else{
		if(toProbNum>numberOfProbes){
			for(var i = fromProbeNumber;i<numberOfProbes;i++){
				readedData.push(eval("values.V"+i.toString()));
			}
		}else{
			for(var i = fromProbeNumber;i<toProbNum;i++){
				readedData.push(eval("values.V"+i.toString()));
			}
		}
		EndFuncAddr(readedData);
		redy = 1;
	}
	
}
function getMeasureValues(Name,min,max,callFunc){
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/measureValues",
		data: {measName: Name, Cmin: min, Cmax: max, numb: Number(Name.match(/\d+/)[0])},
		timeout: 9000,
		success: function(response) {
			numberOfMeasProbes = response[0].Count;
			console.log(response);
			try{
				if(callFunc != undefined) callFunc(Name,response[0].Count,response[0],max);
			}catch(e){
				errorGet = 1;
			}
			lastYear = response[0].year;
			lastMonth = response[0].month;
			lastDay = response[0].day;
			lastHour = response[0].hour;
			lastMinute = response[0].minute;
			lastSecond = response[0].second;
			measureIntervall = response[0].intervall
		},
		error: function(){
			errorGet = 1;
		}
	});	
}

var loadValues = setInterval(function(){
	if(!document.getElementById("plane")==false){
		getSystemVirablesForInfo();
	}
},5000)



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
	
function GenerateDataStringArrayBack(year,month,day,hour,minute,second,intervall,type){
	times = []
	year = Number(year);
	month = Number(month);
	day = Number(day);
	hour = Number(hour);
	minute = Number(minute);
	second = Number(second);
	intervall = Number(intervall);
	szerokosc_okna_wykresu = numOfProbesToRead *30;
	if(szerokosc_okna_wykresu<500) szerokosc_okna_wykresu = 500;

	function pushItoTable(){
		times.push((parseInt(hour, 10) + 100).toString().substr(1) + ":" + (parseInt(minute, 10) + 100).toString().substr(1) + ":" + (parseInt(second, 10) + 100).toString().substr(1)+ " " + (parseInt(day, 10) + 100).toString().substr(1) + "-" + (parseInt(month, 10) + 100).toString().substr(1) + "-" + year.toString());
	}

	for(var i = 0;i<numberOfMeasProbes;i++){


		var y = numberOfMeasProbes - i;
		if(type == 1){
			if(y==1||(y%100 == 99)||(y%100 == 0)||(y == numberOfMeasProbes))
			{ 
				pushItoTable();
			}
		}else{
			//if((toProbeNumber <= numberOfMeasProbes)&&(y>=firstMeasureFromNumber)&&(y<toProbeNumber))  pushItoTable();
			//if((toProbeNumber > numberOfMeasProbes)&&(y>(firstMeasureFromNumber))) pushItoTable();
			
			if((toProbeNumber <= numberOfMeasProbes)&&(y>=firstMeasureFromNumber)&&(y<toProbeNumber)) pushItoTable();
			if((toProbeNumber > numberOfMeasProbes)&&(y>firstMeasureFromNumber)) pushItoTable();
		
		}
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
	
	//var wygenerowane_czasy = GenerateDataStringArrayBack(lastYear,lastMonth,lastDay,lastHour,lastMinute,//lastSecond,measureIntervall,numberOfMeasProbes);
	
	
	
	if(toProbeNumber>numberOfMeasProbes){
		var numberOfDates =  numberOfMeasProbes - (numOfProbesToRead * (page_number));
	}else{
		var numberOfDates = numOfProbesToRead;
	}
	
	var wygenerowane_czasy = GenerateDataStringArrayBack(lastYear,lastMonth,lastDay,lastHour,lastMinute,lastSecond,measureIntervall,numberOfDates);

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
			if(allMeasuredTypes[allMeasuredTypes.length-1] == ""){
				allMeasuredTypes.pop()
			}
			//getAllMeasuresData(allMeasuredTypes);
			drawChecboxes(allMeasuredTypes);
			drawPagesNumbers();
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

function drawPagesNumbers(){
	
	var numOfPages = parseInt(numberOfMeasProbes / numOfProbesToRead);
	
	if(numberOfMeasProbes % numOfProbesToRead){
		numOfPages++;
	}
	
	var wygenerowane_czasy = GenerateDataStringArrayBack(lastYear,lastMonth,lastDay,lastHour,lastMinute,lastSecond,measureIntervall,1);
	
	wygenerowane_czasy = wygenerowane_czasy.reverse();
	
	var tmpStr = "";

	tmpStr += "<div class='pages'>"
	tmpStr += "<select onchange='changePage()' id='pagNumberSelector'>"
	for(var i = 0, y = 0; i<numOfPages;i++,y++){

		tmpStr += "<option value="+i+">"+wygenerowane_czasy[y]+" - "+wygenerowane_czasy[++y]+"</option>"
	}
	tmpStr += "</select>"
	tmpStr += "</div>"

	$('#pagNum').html(tmpStr);
}

function changePage(){
		page_number = Number($("#pagNumberSelector").val());
		drawCheckedCharts();
		drawPagesNumbers();
		$("#pagNumberSelector").val(page_number);
}
//Wizzard
function setVisAutoSec(){
	if($('#autoCont').val() == 0) $('.autoSec').css('display','none');
	else $('.autoSec').css('display','block');
}
function setTimeMarker(){
	dispTimeMarker($('#timeMark').val());
}
function setVisMarker(){
	dispControlMarker($('#controlMark').val());
}
function dispControlMarker(type){
	var tmp = '<br>';
	if(type == 1){
		tmp += "<label>Temperatura</label><br>";
		tmp += generateControlSelector(-50,50,100);
	}else if(type == 2){
		tmp += "<label>Temperatura pokojowa</label><br>";
		tmp += generateControlSelector(15,35,100);			
	}else if(type == 3){
		tmp += "<label>Wilgotność</label><br>";
		tmp += generateControlSelector(0,100,100);		
	}else{}
	
	if(type != 0){
		tmp += '<label>Ekstrema</label><br>'
		tmp += '<input id="controlMinVal" class="form-control" style="float: left; width: 150px;" type="text"></input>'
		tmp += '<input id="controlMaxVal" class="form-control" style="float: right; width: 150px;" type="text"></input>'
	}
	
	$('#plain2').html(tmp);
	addJqueryAttr1();	

}
function dispTimeMarker(type){
	var tmp = "";
	if(type == 1){
		tmp += "<label></label>";
		tmp += generateTimeSelector("day1");
	}else if(type==2){
		tmp += "<label>Poniedziałek</label>";
		tmp += generateTimeSelector("day1");
		tmp += "<label>Wtorek</label>";
		tmp += generateTimeSelector("day2");
		tmp += "<label>Środa</label>";
		tmp += generateTimeSelector("day3");
		tmp += "<label>Czwartek</label>";
		tmp += generateTimeSelector("day4");
		tmp += "<label>Piątek</label>";
		tmp += generateTimeSelector("day5");
		tmp += "<label>Sobota</label>";
		tmp += generateTimeSelector("day6");
		tmp += "<label>Niedziela</label>";
		tmp += generateTimeSelector("day7");
	}
	$('#plane1').html(tmp);
	addJqueryAttr();
}
var nameOfCurrentConfigRelay = "";
function showWizzardWindow(name){
	nameOfCurrentConfigRelay =  name;
	$(".confWindow").css("display","block");
	$("#nameOfrelay").text(name);
}
function generateControlSelector(minVal,maxVal,steps){
		var strDat = minVal;
		var strDat1 = maxVal;
	
		var delta = maxVal - minVal;
		var step = delta/steps;
		var htmlContent = '<br>';
		for(var i = 0;i<=steps;i++){ 
			strDat1 += step;
			htmlContent += '<div value="'+i+'" data-tooltip="'+strDat.toFixed(1)+'" data-tooltip-position="bottom" class="ts1 tp"></div>';
			strDat += step;
		};

		return htmlContent;
}
function addJqueryAttr1(){
		$('.ts1').click(function(){
			selectExtremum(this);
		});
		$('#controlMinVal').change('input propertychange paste', function() {
			selectFromInput();
		});
		$('#controlMaxVal').change('input propertychange paste', function() {
			selectFromInput();
		});
}
function writeExtremumValues(val1,val2){
	$('#controlMinVal').val(val1);
	$('#controlMaxVal').val(val2);
	selectFromInput();
	selectFromInput();
}
function readExtremumValues(){
	var values = [];
	values.push($('#controlMinVal').val())
	values.push($('#controlMaxVal').val())
	return values;
}
function selectFromInput(){
	var divHandlers = $('#plain2').find('div')
	for(var i = 0;i<divHandlers.size();i++){
		if(Number($(divHandlers[i]).attr('data-tooltip'))>=Number($('#controlMinVal').val())){
			selectExtremum(divHandlers[i],1);
			break;
		}
	}
	for(var i = 0;i<divHandlers.size();i++){
		if(Number($(divHandlers[i]).attr('data-tooltip'))>=Number($('#controlMaxVal').val())){
			selectExtremum(divHandlers[i],2);
			break;
		}
	}
}
function selectExtremum(select,MinMax){
	
	//$(select).toggleClass('activeTB')

	$("#plain2").find("div").attr("data-tooltip-position","bottom")
	$("#plain2").find(".activeTB").removeClass("activeTB")
	var elementsHabdlers = [];
	elementsHabdlers = $("#plain2").find('.tsa')
	for(var i = 0;i<elementsHabdlers.size();i++){
		$(elementsHabdlers[i]).removeClass('tsa');
	}
	
	if($("#plain2").find(".firstSel").size() == 0){
		$(select).addClass("firstSel");
	}else{
		//$("#plain2").find(".secondSel").removeClass("activeTB")
		$("#plain2").find(".secondSel").removeClass("secondSel")
		var firstSel = $("#plain2").find(".firstSel");
		$(firstSel).addClass("secondSel");
		$(firstSel).removeClass("firstSel");
		$(select).addClass("firstSel");
	}
	
	var min;
	var max;
	
	var first = Number($($("#plain2").find(".secondSel")[0]).attr("value"));
	var second = Number($($("#plain2").find(".firstSel")[0]).attr("value"));
	
	if(first > second ){
		max = first;
		min = second;
	}else{
		max = second;
		min = first;
	}
	
	var delta = max - min;
	
	for(var i = 0; i<delta+1; i++){
		$($('#plain2 div')[min + i]).addClass("tsa");
	}
	
	if(MinMax != 2) $('#controlMinVal').val($($('#plain2 div')[min]).attr('data-tooltip'));
	if(MinMax != 1) $('#controlMaxVal').val($($('#plain2 div')[max]).attr('data-tooltip'));
	
	elementsHabdlers = $("#plain2").find('div')
	
	$(elementsHabdlers[min]).addClass('activeTB');
	$(elementsHabdlers[max]).addClass('activeTB');
	$("#plain2").find(".activeTB").attr("data-tooltip-position","top");
}
	var mouseDown = 0;
	
	document.body.onmousedown = function() { 
	  mouseDown = 1;
	}
	document.body.onmouseup = function() {
	  mouseDown = 0;
	}
	

var trash = '<svg width="25" height="25" viewBox="0 0 1792 1792" xmlns="http://www.w3.org/2000/svg"><path d="M704 736v576q0 14-9 23t-23 9h-64q-14 0-23-9t-9-23v-576q0-14 9-23t23-9h64q14 0 23 9t9 23zm256 0v576q0 14-9 23t-23 9h-64q-14 0-23-9t-9-23v-576q0-14 9-23t23-9h64q14 0 23 9t9 23zm256 0v576q0 14-9 23t-23 9h-64q-14 0-23-9t-9-23v-576q0-14 9-23t23-9h64q14 0 23 9t9 23zm128 724v-948h-896v948q0 22 7 40.5t14.5 27 10.5 8.5h832q3 0 10.5-8.5t14.5-27 7-40.5zm-672-1076h448l-48-117q-7-9-17-11h-317q-10 2-17 11zm928 32v64q0 14-9 23t-23 9h-96v948q0 83-47 143.5t-113 60.5h-832q-66 0-113-58.5t-47-141.5v-952h-96q-14 0-23-9t-9-23v-64q0-14 9-23t23-9h309l70-167q15-37 54-63t79-26h320q40 0 79 26t54 63l70 167h309q14 0 23 9t9 23z"/></svg>'

var list = '<svg width="25" height="25" viewBox="0 0 1792 1792" xmlns="http://www.w3.org/2000/svg"><path d="M381 1620q0 80-54.5 126t-135.5 46q-106 0-172-66l57-88q49 45 106 45 29 0 50.5-14.5t21.5-42.5q0-64-105-56l-26-56q8-10 32.5-43.5t42.5-54 37-38.5v-1q-16 0-48.5 1t-48.5 1v53h-106v-152h333v88l-95 115q51 12 81 49t30 88zm2-627v159h-362q-6-36-6-54 0-51 23.5-93t56.5-68 66-47.5 56.5-43.5 23.5-45q0-25-14.5-38.5t-39.5-13.5q-46 0-81 58l-85-59q24-51 71.5-79.5t105.5-28.5q73 0 123 41.5t50 112.5q0 50-34 91.5t-75 64.5-75.5 50.5-35.5 52.5h127v-60h105zm1409 319v192q0 13-9.5 22.5t-22.5 9.5h-1216q-13 0-22.5-9.5t-9.5-22.5v-192q0-14 9-23t23-9h1216q13 0 22.5 9.5t9.5 22.5zm-1408-899v99h-335v-99h107q0-41 .5-122t.5-121v-12h-2q-8 17-50 54l-71-76 136-127h106v404h108zm1408 387v192q0 13-9.5 22.5t-22.5 9.5h-1216q-13 0-22.5-9.5t-9.5-22.5v-192q0-14 9-23t23-9h1216q13 0 22.5 9.5t9.5 22.5zm0-512v192q0 13-9.5 22.5t-22.5 9.5h-1216q-13 0-22.5-9.5t-9.5-22.5v-192q0-13 9.5-22.5t22.5-9.5h1216q13 0 22.5 9.5t9.5 22.5z"/></svg>'

function clearAllTimes(fieldId,timeSelField){
	selectTimes('#'+$(timeSelField).attr('id'),0, 0)
	$('#' + $(fieldId).attr('id')).empty();
}
	function generateTimeSelector(fieldId){
		var date = new Date(0);
		date.setHours(0);
		var strDat = '';
		
		var date1 = new Date(0);
		date1.setHours(0);
		var strDat1 = '';
	
		var htmlContent = '<div id="'+fieldId+'" class="timeSel">';
		for(var i = 0;i<96;i++){ 
			strDat =  date.getHours() + ":" + date.getMinutes();
			date1.setMinutes(date.getMinutes() + 15);
			strDat1 =  date1.getHours() + ":" + date1.getMinutes();
			date.setMinutes(date.getMinutes() + 15);

			htmlContent += '<span data-tooltip="'+strDat+'" data1-tooltip="'+strDat1+'" data-tooltip-position="bottom" class="ts tp"></span>';
		};
		htmlContent +='<span data-tooltip="24:00" data1-tooltip="24:00" data-tooltip-position="bottom" class="ts empty tp"></span>'
		
		htmlContent +='<span style="cursor: pointer;" onclick="clearAllTimes(listOf'+nameOfCurrentConfigRelay+fieldId+','+fieldId+')">'
		htmlContent += trash
		htmlContent += '</span>'
		htmlContent += '<span style="cursor: pointer;" onclick="toggleListVis(timeListField'+nameOfCurrentConfigRelay+fieldId+')">'
		htmlContent += list
		htmlContent += '</span>'
		htmlContent += '</div><div id="timeListField'+nameOfCurrentConfigRelay+fieldId+'">'
		htmlContent += '<select id="listOf'+nameOfCurrentConfigRelay+fieldId+'" class="timelist" multiple></select><br>'
		htmlContent += '<input id="startTime'+fieldId+'" type="text" style="float: left"></input>'
		htmlContent += '<input id="endTime'+fieldId+'" type="text" style="float: right"></input><br>'
		htmlContent += '<div class="buttblock"><input onclick=deleteSelectedTimesFromMultipleSelector("#listOf'+nameOfCurrentConfigRelay+fieldId+'","#'+fieldId+'") class="form-control" type="button" value="Usuń">'
		htmlContent += '<input onclick=addTimeToSelector("#listOf'+nameOfCurrentConfigRelay+fieldId+'",'+fieldId+') class="form-control" type="button" value="Dodaj">'
		htmlContent += '</div></div>'
		
		setTimeout(function(){eval('$("#timeListField'+nameOfCurrentConfigRelay+fieldId+'").hide()')},100);
		
		return htmlContent;
	}
	function addTimeToSelector(nameOfSelector,fieldId){
		
		startTime = $('#startTime'+$(fieldId).attr('id')).val()
		endTime = $('#endTime'+$(fieldId).attr('id')).val()
		writeTimeToMultipleSelector(nameOfSelector,startTime,endTime)
		
		var startTimes = [];
		var endTimes = [];
		for(var i = 0;i<$(nameOfSelector).find('option').length;i++){
			var times = $($(nameOfSelector).find('option')[i]).val().split(" ");
			startTimes.push(times[0]);
			endTimes.push(times[2]);
		}
		selectTimes(fieldId,startTimes, endTimes);
		
	}
	
	function toggleListVis(listId){
		$(listId).slideToggle('slow');
	}
	function addJqueryAttr(){
		$('.ts').mouseover(function(){if(mouseDown){$(this).toggleClass('tsa')}})
		$('.ts').hover(function(){if(mouseDown){$(this).toggleClass('tsa')}})
		$('.ts').click(function(){
			$(this).toggleClass('tsa');
			writeTimesToMultiple($($(this).parent()).attr('id'));
		})
		$('.ts').mouseenter(function(){$(this).addClass('activeTB')})
		$('.ts').mouseleave(function(){
			$(this).removeClass('activeTB');
			if(mouseDown) writeTimesToMultiple($($(this).parent()).attr('id'));
		})
	}
	function writeTimesToMultiple(fieldId){
		var statEndTimes = SelectedTimes('#'+fieldId);
		$('#listOf'+nameOfCurrentConfigRelay+fieldId).empty();
		for(var i = 0 ; i<statEndTimes.startTimes.length ; i++){
			writeTimeToMultipleSelector( '#listOf'+nameOfCurrentConfigRelay+fieldId , statEndTimes.startTimes[i] , statEndTimes.endTimes[i]);
		}
	}
	function SelectedTimes(fieldId){
		var startTimes = [];
		var endTimes = [];
		var allTimes = [];
		var date = new Date();
		
		allTimes = $(fieldId).children()
		for(var i = 0; i < allTimes.size()-1;i++){
			if(i == 0){
				if($(allTimes[0]).hasClass('tsa')) startTimes.push(allTimes[0].getAttribute('data-tooltip'));
			}else{
				if($(allTimes[i]).hasClass('tsa') && ($(allTimes[i-1]).hasClass('tsa') == false))  startTimes.push(allTimes[i].getAttribute('data-tooltip'));
			}
			if(i == (allTimes.size() -  1)){
				if($(allTimes[allTimes.size() -  1]).hasClass('tsa')) endTimes.push('00:00');
			}else{
				if($(allTimes[i]).hasClass('tsa') && ($(allTimes[i+1]).hasClass('tsa') == false)){
					date.setHours(allTimes[i].getAttribute('data-tooltip').split(':')[0]);
					date.setMinutes(allTimes[i].getAttribute('data-tooltip').split(':')[1]);
					date.setTime(date.getTime()+15*60000);
					endTimes.push(date.getHours()+':'+date.getMinutes());
				}
			}
		}		
		var times = {startTimes,endTimes};
		return times;
	}
	function selectTimes(fieldId,startTimes, endTimes){
		snaps = $(fieldId).find('span');
	
		for(var i = 0;i<snaps.length;i++){
			$(snaps[i]).removeClass('tsa');
		}

		
		for(var y = 0; y< startTimes.length;y++){
			var spanTime = new Date(0, 0, 0, 0, 0, 0, 0);
			
			startHours = Number(startTimes[y].split(':')[0]);
			startMinutes = Number(startTimes[y].split(':')[1]);
			
			endHours = Number(endTimes[y].split(':')[0]);
			endMinutes = Number(endTimes[y].split(':')[1]);
			
			var compareStartTime = new Date(0, 0, 0, startHours, startMinutes, 0, 0);
			var compareEndTime = new Date(0, 0, 0, endHours, endMinutes, 0, 0);
			
			
			for(var i = 0;i<snaps.length;i++){
				if((spanTime.getTime()>=compareStartTime.getTime())&&(spanTime.getTime()<compareEndTime.getTime())){
					$(snaps[i]).addClass('tsa');
				}
				spanTime.setTime(spanTime.getTime()+15*60000);
			}
		}
		writeTimesToMultiple(fieldId.substring(1));
	}

var cloud = '<svg class="valueIcon" width="120" height="90" viewBox="0 0 1792 1792" xmlns="http://www.w3.org/2000/svg"><path d="M1856 1152q0 159-112.5 271.5t-271.5 112.5h-1088q-185 0-316.5-131.5t-131.5-316.5q0-132 71-241.5t187-163.5q-2-28-2-43 0-212 150-362t362-150q158 0 286.5 88t187.5 230q70-62 166-62 106 0 181 75t75 181q0 75-41 138 129 30 213 134.5t84 239.5z"/></svg>'
var sun = '<svg class="valueIcon" width="120" height="90" viewBox="0 0 1792 1792" xmlns="http://www.w3.org/2000/svg"><path d="M1472 896q0-117-45.5-223.5t-123-184-184-123-223.5-45.5-223.5 45.5-184 123-123 184-45.5 223.5 45.5 223.5 123 184 184 123 223.5 45.5 223.5-45.5 184-123 123-184 45.5-223.5zm276 277q-4 15-20 20l-292 96v306q0 16-13 26-15 10-29 4l-292-94-180 248q-10 13-26 13t-26-13l-180-248-292 94q-14 6-29-4-13-10-13-26v-306l-292-96q-16-5-20-20-5-17 4-29l180-248-180-248q-9-13-4-29 4-15 20-20l292-96v-306q0-16 13-26 15-10 29-4l292 94 180-248q9-12 26-12t26 12l180 248 292-94q14-6 29 4 13 10 13 26v306l292 96q16 5 20 20 5 16-4 29l-180 248 180 248q9 12 4 29z"/></svg>'
var tint = '<svg class="valueIcon" width="120" height="90" viewBox="0 0 1792 1792" xmlns="http://www.w3.org/2000/svg"><path d="M896 1152q0-36-20-69-1-1-15.5-22.5t-25.5-38-25-44-21-50.5q-4-16-21-16t-21 16q-7 23-21 50.5t-25 44-25.5 38-15.5 22.5q-20 33-20 69 0 53 37.5 90.5t90.5 37.5 90.5-37.5 37.5-90.5zm512-128q0 212-150 362t-362 150-362-150-150-362q0-145 81-275 6-9 62.5-90.5t101-151 99.5-178 83-201.5q9-30 34-47t51-17 51.5 17 33.5 47q28 93 83 201.5t99.5 178 101 151 62.5 90.5q81 127 81 275z"/></svg>'

function kafelek(name){
	this.state = 0;     //Relay 0 - off, 1 - on
	this.name = name;   //name of element
	this.auto = 0;		//Automatic control, 0-off 1 -on
	this.value = 0;     //Value of variable
	this.icon = cloud;
	this.unit = "";
	
	this.color = ""
	//state - kolor kafelka 1 - red,0- green
	//auto - tryb automatyczny 1 - on, 0 - off
	this.toggleColorState = function(state,auto){
		this.state = state;
		this.auto = auto;
		if(this.state == 0){
			eval('$("#'+name+' div:first-child").removeClass("red");')
			eval('$("#'+name+' div:first-child").addClass("green")')
		}else{
			eval('$("#'+name+' div:first-child").removeClass("green");')
			eval('$("#'+name+' div:first-child").addClass("red")')
		}
		if(this.auto == 0){
			eval('$("#'+name+' div:first-child").next().attr("class","icon")');
		}else{
			eval('$("#'+name+' div:first-child").next().attr("class","icon rotate")');
		}
	}
	//type - 0 relay config
	this.setVal = function(value1){
		this.value = value1;
		$("#" + this.name + " text:nth-of-type(2n)").text(this.value.toString())
	}
	this.draw = function(type){
		if(type == 0){
			var htmlString = '<div id="'+name+'" class="container2">';
			if(this.state == 0){
				this.color = "green";
			}else{
				this.color = "red";
			}
			htmlString += '<div class="square elegantBlock '+ this.color +'" onclick="' + this.name +'.checkState()"><text class="boxText">'+name+'</text></div>';
			htmlString += '<span class="ico"><svg onclick=relaywizzardWasClicked("'+name+'") class="icon rotate" width="30" height="30" viewBox="0 0 1792 1792" xmlns="http://www.w3.org/2000/svg"><path d="M1152 896q0-106-75-181t-181-75-181 75-75 181 75 181 181 75 181-75 75-181zm512-109v222q0 12-8 23t-20 13l-185 28q-19 54-39 91 35 50 107 138 10 12 10 25t-9 23q-27 37-99 108t-94 71q-12 0-26-9l-138-108q-44 23-91 38-16 136-29 186-7 28-36 28h-222q-14 0-24.5-8.5t-11.5-21.5l-28-184q-49-16-90-37l-141 107q-10 9-25 9-14 0-25-11-126-114-165-168-7-10-7-23 0-12 8-23 15-21 51-66.5t54-70.5q-27-50-41-99l-183-27q-13-2-21-12.5t-8-23.5v-222q0-12 8-23t19-13l186-28q14-46 39-92-40-57-107-138-10-12-10-24 0-10 9-23 26-36 98.5-107.5t94.5-71.5q13 0 26 10l138 107q44-23 91-38 16-136 29-186 7-28 36-28h222q14 0 24.5 8.5t11.5 21.5l28 184q49 16 90 37l142-107q9-9 24-9 13 0 25 10 129 119 165 170 7 8 7 22 0 12-8 23-15 21-51 66.5t-54 70.5q26 50 41 98l183 28q13 2 21 12.5t8 23.5z"/></svg></span>';
			htmlString += '</div>';
			return htmlString;
		}
		if(type > 0){
			if(type == 1){
				this.color = "orange";
				this.icon = sun
				this.unit = "°C"
			}else if(type == 2){
				this.color = "grey";
				this.icon = cloud
				this.unit = "%"
			}else if(type == 3){
				this.color = "blue";
				this.icon = tint
				this.unit = "%"
			}
			var htmlString = '<div id='+name+' class="container1">';
			htmlString += '<div class="value elegantBlock '+this.color+'">';
			htmlString += this.icon;
			htmlString += '<text class="boxText">&nbsp;&nbsp;'+name+'&nbsp;&nbsp;</text><text class="boxText">'+ this.value +'</text><text class="boxText">'+ this.unit +'</text>'
			htmlString += '</div></div>'
			return htmlString;
		}
	}
}
function writeTimeToMultipleSelector(nameOfSelector,startValue,endValue){
	$(nameOfSelector).append('<option>'+startValue+' - '+endValue+'</option>');
}
function deleteSelectedTimesFromMultipleSelector(selectorId,fieldId){
	var optionValues = $(selectorId).val()
	
	for(var i = 0; i< $(selectorId).find('option').length;i++){
		for(var y = 0; y < optionValues.length;y++){
			if($($(selectorId).find('option')[i]).val() == optionValues[y]){
				$($(selectorId).find('option')[i]).remove()
			}
		}
	}
	var startTimes = [];
	var endTimes = [];
	for(var i = 0;i<$(selectorId).find('option').length;i++){
		var times = $($(selectorId).find('option')[i]).val().split(" ");
		startTimes.push(times[0]);
		endTimes.push(times[2]);
	}
	selectTimes(fieldId,startTimes, endTimes);
}

function arrayToString(array){
	var string = "";
	for(var y = 0; y<array.length;y++){
		string += array[y] + ";";
	}	
	return string
}
function sendControlRelaySetting(){
	var data = {};
	data['fileName'] = nameOfCurrentConfigRelay;
	data['nameOfRelay'] = nameOfCurrentConfigRelay;
	data['timeMark'] = $('#timeMark').val();
	data['controlMark'] = $('#controlMark').val();
	if(($('#autoCont').val() == 1)&&($('#timeMark').val() == 1)){
		data['day1'] = SelectedTimes("#day1");
		data['day1'].startTimes = arrayToString(data['day1'].startTimes);
		data['day1'].endTimes = arrayToString(data['day1'].endTimes);
	}
	if(($('#autoCont').val() == 1)&&($('#timeMark').val() == 2)){
		for(var i = 1; i<=7;i++){
			eval("data['day"+i+"'] = SelectedTimes('#day"+i+"');");
			eval("data['day"+i+"'].startTimes = arrayToString(data['day"+i+"'].startTimes);");
			eval("data['day"+i+"'].endTimes = arrayToString(data['day"+i+"'].endTimes);");
		}
	}
	if($('#controlMark').val() != 0){
		var tmpVal = readExtremumValues();
		data['typeOfControl'] = {"minVal" : tmpVal[0],"maxVal" : tmpVal[1]};
	}
	
	saveAjaxStructToFile(data);
}
function saveAjaxStructToFile(struct){
	$.ajax({
	  method: "POST",
	  url: "saveAjaxStructToFile",
	  data: struct
	})
	.done(function( msg ) {
		alert( msg );
	});
}
function readAjaxStructFromFile(fileName){
	var data = "";
	
	var struct = {};
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/readAjaxStructFromFile",
		data: {fileName: fileName},
		success: function(response) {
			data = response.contentStr;
			var elements = data.split("#$");
			var loopChecker = "";
			for(var i = 0; i<elements.length-1; i++){
				if(elements[i].search("%5B") == -1){
					var tmpData = elements[i].split("#:");
					struct[tmpData[0].toString()] = tmpData[1].toString()
				}else if(elements[i].search("%5B%5D") != -1){
					var tmpData = elements[i].split("%5B%5D#:");
					struct[tmpData[0].toString()] = tmpData[1].toString()
				}else{
					var tmpData = [];
					var firstSeparator = elements[i].search("%5B");
					var secondSeparator = elements[i].search("%5D#:");
					
					tmpData.push(elements[i].substring(0,firstSeparator));
					tmpData.push(elements[i].substring(firstSeparator+3,secondSeparator));
					tmpData.push(elements[i].substring(secondSeparator+5));
					
					if(loopChecker != tmpData[0].toString()){
						struct[tmpData[0].toString()] = {};
						loopChecker = tmpData[0].toString();
					}
					
					struct[tmpData[0].toString()][tmpData[1].toString()]= tmpData[2].toString();
				}
			}
			setReadedSettings(struct);
		},
		error: function(response){
		
		}
	});
	
	return data;
}
//Uniwersalna funkcja odczytu dla całego projektu
function setReadedSettings(struct){
	if(struct.fileName.search("Relay") != -1){
		$('#timeMark').val(struct.timeMark);
		$('#controlMark').val(struct.controlMark);
		setVisAutoSec();
		setTimeMarker();
		setVisMarker();
		if(struct.timeMark == "1"){
			var startTimes = struct.day1.startTimes.split(";");
			startTimes.pop();
			
			var endTimes = struct.day1.endTimes.split(";");
			endTimes.pop();
			
			selectTimes('#day1',startTimes,endTimes);
			
		}else if(struct.timeMark == "2"){
			for(var z = 1; z<=7;z++){
				eval('var startTimes = struct.day'+z+'.startTimes.split(";");');
				eval('startTimes.pop();');
				
				eval('var endTimes = struct.day'+z+'.endTimes.split(";");');
				eval('endTimes.pop();');
				
				eval('selectTimes("#day'+z+'",startTimes,endTimes);');
			}
		}
		if(struct.controlMark != 0){
			writeExtremumValues(Number(struct.typeOfControl.minVal),Number(struct.typeOfControl.maxVal));
			writeExtremumValues(Number(struct.typeOfControl.minVal),Number(struct.typeOfControl.maxVal));
		}
	}
}
function relaywizzardWasClicked(name){
	showWizzardWindow(name);
	readAjaxStructFromFile(nameOfCurrentConfigRelay);
}
