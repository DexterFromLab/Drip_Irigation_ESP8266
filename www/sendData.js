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
			second: date[5]
			}
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
			$("#tempMax").val(response.tempMax);
			$("#tempMaxOn").val(response.tempMaxOn);
			$("#tempMin").val(response.tempMin);
			$("#tempMinOn").val(response.tempMinOn);
			$("#DeltaT").val(response.DeltaT);
			$("#DeltaTim").val(response.DeltaTim);
			$("#DeltaRelayTime").val(response.DeltaRelayTime);
			$("#DeltaTOn").val(response.DeltaTOn);
		}
	});
}
function getTimSterSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/timSter",
		success: function(response) {
			$("#slider-range1").slider('values',0,response.tim1Min)
			$("#slider-range1").slider('values',1,response.tim1Max)
			$("#time1State").val(response.tim1);
			$("#slider-range2").slider('values',1,response.tim2Min)
			$("#slider-range2").slider('values',0,response.tim2Max)
			$("#time2State").val(response.tim2);
			$("#slider-range3").slider('values',1,response.tim3Min)
			$("#slider-range3").slider('values',0,response.tim3Max)
			$("#time3State").val(response.tim3);
		}
	});
}
function getAirhumSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/airhum",
		success: function(response) {
			$("#wilgPowMax").val(response.wilgPowMax);
			$("#wilgPowMaxOn").val(response.wilgPowMaxOn);
			$("#wilgPowMin").val(response.wilgPowMin);
			$("#wilgPowMinOn").val(response.wilgPowMinOn);
			$("#DeltaWilgPow").val(response.DeltaWilgPow);
			$("#DeltaWilgPowTim").val(response.DeltaWilgPowTim);
			$("#DeltaWilgPowRelayTim").val(response.DeltaWilgPowRelayTim);
			$("#DeltaWilgPowOn").val(response.DeltaWilgPowOn);
		}
	});
}
function getHumSettings() {
	$.ajax({
		type: "GET",
		datatype: "html",
		url: "/hum",
		success: function(response) {
			$("#wilgMax").val(response.wilgMax);
			$("#wilgMaxOn").val(response.wilgMaxOn);
			$("#wilgMin").val(response.wilgMin);
			$("#wilgMinOn").val(response.wilgMinOn);
			$("#DeltaWilg").val(response.DeltaWilg);
			$("#DeltaWilgTim").val(response.DeltaWilgTim);
			$("#DeltaWilgRelayTim").val(response.DeltaWilgRelayTim);
			$("#DeltaWilgOn").val(response.DeltaWilgOn);
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
			$("#dns").val(
				response.dns0.toString() + "."+
				response.dns1.toString() + "."+
				response.dns2.toString() + "."+
				response.dns3.toString()
			);
			$("#dhcpOn").val(response.dhcpOn);
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
		}
	});
}
function getTimeField() {
	var date = new Array(
		Number($('#date').val().substr(0,4)),
		Number($('#date').val().substr(5,2)),
		Number($('#date').val().substr(8,2)),
		Number($('#time').val().substr(0,2)),
		Number($('#time').val().substr(3,2)),
		Number($('#time').val().substr(6,2))
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