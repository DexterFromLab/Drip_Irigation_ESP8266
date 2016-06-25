var TimMin = new Array(0,0,0);
var TimMax = new Array(0,0,0);

function setSlider(name,num){
	$(name + num.toString()).slider({
		range: true,
		min: 0,
		max: 1440,
		step: 15,
		values: [600, 720],
		slide: function (e, ui) {
			var hours1 = Math.floor(ui.values[0] / 60);
			var minutes1 = ui.values[0] - (hours1 * 60);
			
			TimMin[num - 1] = ui.values[0];
			
			if (hours1.length == 1) hours1 = '0' + hours1;
			if (minutes1.length == 1) minutes1 = '0' + minutes1;
			if (minutes1 == 0) minutes1 = '00';
			if (hours1 >= 12) {
				if (hours1 == 12) {
					hours1 = hours1;
					minutes1 = minutes1 + " PM";
				} else {
					hours1 = hours1 - 12;
					minutes1 = minutes1 + " PM";
				}
			} else {
				hours1 = hours1;
				minutes1 = minutes1 + " AM";
			}
			if (hours1 == 0) {
				hours1 = 12;
				minutes1 = minutes1;
			}



			$('#slider'+num.toString()+'-time').html(hours1 + ':' + minutes1);

			var hours2 = Math.floor(ui.values[1] / 60);
			var minutes2 = ui.values[1] - (hours2 * 60);

			TimMax[num - 1] = ui.values[1];
			
			if (hours2.length == 1) hours2 = '0' + hours2;
			if (minutes2.length == 1) minutes2 = '0' + minutes2;
			if (minutes2 == 0) minutes2 = '00';
			if (hours2 >= 12) {
				if (hours2 == 12) {
					hours2 = hours2;
					minutes2 = minutes2 + " PM";
				} else if (hours2 == 24) {
					hours2 = 11;
					minutes2 = "59 PM";
				} else {
					hours2 = hours2 - 12;
					minutes2 = minutes2 + " PM";
				}
			} else {
				hours2 = hours2;
				minutes2 = minutes2 + " AM";
			}

			$('#slider'+num.toString()+'-time2').html(hours2 + ':' + minutes2);
		}
	});
}
function sendCzasy(){
	$.ajax({
	  method: "POST",
	  url: "timSter",
	  data: { Tim1Min: $("#slider-range1").slider('values',0), Tim1Max: $("#slider-range1").slider('values',1),Tim1Stat: Number($("#time1State").val()), Tim2Min: $("#slider-range2").slider('values',0), Tim2Max: $("#slider-range2").slider('values',1),Tim2Stat: Number($("#time2State").val()), Tim3Min: $("#slider-range3").slider('values',0), Tim3Max: $("#slider-range3").slider('values',1),Tim3Stat: Number($("#time3State").val()) }
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
function sendTemp(){
	$.ajax({
	  method: "POST",
	  url: "temperature",
	  data: { tempMax: $("#tempMax").val(), tempMaxOn: $("#tempMaxOn").val(), tempMin: $("#tempMin").val(), tempMinOn: $("#tempMinOn").val(), DeltaT: $("#DeltaT").val(), DeltaTim: $("#DeltaTim").val(), DeltaRelayTime: $("#DeltaRelayTime").val(), DeltaTOn: $("#DeltaTOn").val() }
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
function sendWilgPow(){
	$.ajax({
	  method: "POST",
	  url: "airHum",
	  data: { wilgPowMax: $("#wilgPowMax").val(), wilgPowMaxOn: $("#wilgPowMaxOn").val(), wilgPowMin: $("#wilgPowMin").val(), wilgPowMinOn: $("#wilgPowMinOn").val(), DeltaWilgPow: $("#DeltaWilgPow").val(), DeltaWilgPowTim: $("#DeltaWilgPowTim").val(), DeltaWilgPowRelayTim: $("#DeltaWilgPowRelayTim").val(), DeltaWilgPowOn: $("#DeltaWilgPowOn").val() }
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
function sendWilgGl(){
	$.ajax({
	  method: "POST",
	  url: "Hum",
	  data: { wilgMax: $("#wilgMax").val(), wilgMaxOn: $("#wilgMaxOn").val(), wilgMin: $("#wilgMin").val(), wilgMinOn: $("#wilgMinOn").val(), DeltaWilg: $("#DeltaWilg").val(), DeltaWilgTim: $("#DeltaWilgTim").val(), DeltaWilgRelayTim: $("#DeltaWilgRelayTim").val(), DeltaWilgOn: $("#DeltaWilgOn").val() }
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
function sendInter(){
	$.ajax({
	  method: "POST",
	  url: "ethernet",
	  data: { ip0: $("#ip").val().substr(0,3), ip1: $("#ip").val().substr(4,3), ip2: $("#ip").val().substr(8,3), ip3: $("#ip").val().substr(12,3), m0: $("#m").val().substr(0,3), m1: $("#m").val().substr(4,3), m2: $("#m").val().substr(8,3), m3: $("#m").val().substr(12,3),gat0: $("#gat").val().substr(0,3), gat1: $("#gat").val().substr(4,3), gat2: $("#gat").val().substr(8,3), gat3: $("#gat").val().substr(12,3), dns0: $("#dns").val().substr(0,3), dns1: $("#dns").val().substr(4,3), dns2: $("#dns").val().substr(8,3), dns3: $("#dns").val().substr(12,3), dhcpOn: $("#dhcpOn").val() }
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
function sendSys(){
	$.ajax({
	  method: "POST",
	  url: "system",
	  data: { workMod: $("#workMod").val(),  chandModeOff: $("#chandModeOff").val(), chandModeHumTimeEnd: $("#chandModeHumTimeEnd").val(), timeMod: $("#timeMod").val(), NTP_addr: $("#NTP_addr").val(), POSIX_time: getTimeField()}
	})
	.done(function( msg ) {
		alert( "Data Saved: " + msg );
	});
}
function getTemperatureSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/temperature",
		success: function(response) {

		}
	});
}
function getTimSterSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/timSter",
		success: function(response) {

		}
	});
}
function getAirhumSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/airhum",
		success: function(response) {

		}
	});
}
function getHumSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/hum",
		success: function(response) {

		}
	});
}
function getEthernetSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/ethernet",
		success: function(response) {

		}
	});
}
function getSystemSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/system",
		success: function(response) {

		}
	});
}
function getTimeField() {
	var date = new Date(
	Number($('#date').val().substr(0,4)),
	Number($('#date').val().substr(5,2)),
	Number($('#date').val().substr(8,2)),
	Number($('#time').val().substr(0,2)),
	Number($('#time').val().substr(3,2)),
	Number($('#time').val().substr(6,2))
	);
	return date.getTime();
}
function fillTimeField(posix_time){
	var date = new Date(posix_time);
	$('#date').val(
		date.getFullYear().toString() + "-" +
		padLeft((date.getMonth()+1),2) + "-" + 
		padLeft(date.getDate(),2)
	);
	$('#time').val(
		padLeft(date.getHours().toString(),2) + ":" +
		padLeft(date.getMinutes().toString(),2) + ":" +
		padLeft(date.getSeconds().toString(),2)
	)
}
function padLeft(nr, n, str){
    return Array(n-String(nr).length+1).join(str||'0')+nr;
}