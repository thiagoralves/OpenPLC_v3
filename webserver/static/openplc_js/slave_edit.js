var tcpdiv = document.getElementById("tcp-stuff");
var rtudiv = document.getElementById("rtu-stuff");
var frm = document.getElementById('slave_form');
var dev_name = document.getElementById("dev_name");
var dev_type = document.getElementById('dev_type');
var dev_ip = document.getElementById("ip_address");
var dev_port = document.getElementById("ip_port");
var slave_id = document.getElementById("slave_id");
var dev_baud = document.getElementById("baud_rate");
var dev_parity = document.getElementById("parity");
var dev_data = document.getElementById("data_bits");
var dev_stop = document.getElementById("stop_bits");
var di_start = document.getElementById("di_start");
var di_size = document.getElementById("di_size");
var do_start = document.getElementById("coil_start");
var do_size = document.getElementById("coil_size");
var ai_start = document.getElementById("ir_start");
var ai_size = document.getElementById("ir_size");
var aor_start = document.getElementById("hr_read_start");
var aor_size = document.getElementById("hr_read_size");
var aow_start = document.getElementById("hr_write_start");
var aow_size = document.getElementById("hr_write_size");
window.onload = function () {
    //updateFormWidgets();
    //LoadValuesFromDB()
};
function turnElementOn(element) {
    element.readOnly = false;
    element.value = "";
    //element.style.backgroundColor = "white"
    //element.style.color = "black"
}
function turnElementOff(element) {
    element.readOnly = true;
    // element.style.backgroundColor = "#F8F8F8"
    //element.style.color = "#9C9C9C"
}
dev_type.onchange = function () {
    //updateFormWidgets();
};
function updateFormWidgets() {
    ssss;
    // switch all elements off, then enable on option below
    for (var _i = 0, _a = [dev_port, slave_id, di_start, di_size, do_start, do_size, ai_start, ai_size, aor_start, aor_size, aow_start, aow_size]; _i < _a.length; _i++) {
        var ele = _a[_i];
        turnElementOff(ele);
    }
    var dtype = dev_type.options[dev_type.selectedIndex].value;
    console.log(dtype);
    switch (dtype) {
        case "TCP":
            $(tcpdiv).show();
            $(rtudiv).hide();
            turnElementOn(dev_port);
            turnElementOn(slave_id);
            turnElementOn(di_start);
            turnElementOn(di_size);
            turnElementOn(do_start);
            turnElementOn(do_size);
            turnElementOn(ai_start);
            turnElementOn(ai_size);
            turnElementOn(aor_start);
            turnElementOn(aor_size);
            turnElementOn(aow_start);
            turnElementOn(aow_size);
            break;
        case "RTU":
            $(tcpdiv).hide();
            $(rtudiv).show();
            turnElementOn(slave_id);
            turnElementOn(dev_baud);
            turnElementOn(dev_parity);
            dev_parity.value = "None";
            turnElementOn(dev_data);
            turnElementOn(dev_stop);
            turnElementOn(di_start);
            turnElementOn(di_size);
            turnElementOn(do_start);
            turnElementOn(do_size);
            turnElementOn(ai_start);
            turnElementOn(ai_size);
            turnElementOn(aor_start);
            turnElementOn(aor_size);
            turnElementOn(aow_start);
            turnElementOn(aow_size);
            break;
        case "ESP32":
            $(tcpdiv).show();
            $(rtudiv).hide();
            dev_port.value = "502";
            slave_id.value = "0";
            di_start.value = "0";
            di_size.value = "8";
            do_start.value = "0";
            do_size.value = "8";
            ai_start.value = "0";
            ai_size.value = "1";
            aor_start.value = "0";
            aor_size.value = "0";
            aow_start.value = "0";
            aow_size.value = "1";
            break;
        case "ESP8266":
            $(tcpdiv).show();
            $(rtudiv).hide();
            dev_port.value = "502";
            slave_id.value = "0";
            di_start.value = "0";
            di_size.value = "8";
            do_start.value = "0";
            do_size.value = "8";
            ai_start.value = "0";
            ai_size.value = "1";
            aor_start.value = "0";
            aor_size.value = "0";
            aow_start.value = "0";
            aow_size.value = "1";
            break;
        case "Uno":
            $(tcpdiv).hide();
            $(rtudiv).show();
            slave_id.value = "0";
            dev_baud.value = "115200";
            dev_parity.value = "None";
            dev_data.value = "8";
            dev_stop.value = "1";
            di_start.value = "0";
            di_size.value = "5";
            do_start.value = "0";
            do_size.value = "4";
            ai_start.value = "0";
            ai_size.value = "6";
            aor_start.value = "0";
            aor_size.value = "0";
            aow_start.value = "0";
            aow_size.value = "3";
            break;
        case "Mega":
            $(tcpdiv).hide();
            $(rtudiv).show();
            slave_id.value = "0";
            dev_baud.value = "115200";
            dev_parity.value = "None";
            dev_data.value = "8";
            dev_stop.value = "1";
            di_start.value = "0";
            di_size.value = "24";
            do_start.value = "0";
            do_size.value = "16";
            ai_start.value = "0";
            ai_size.value = "16";
            aor_start.value = "0";
            aor_size.value = "0";
            aow_start.value = "0";
            aow_size.value = "12";
            break;
    }
}
function validateForm() {
    if (dev_name.value == "" || slave_id.value == "") {
        alert("Please fill out all the fields before saving!");
        return false;
    }
    var device_type = dev_protocol.options[dev_protocol.selectedIndex].value;
    if (device_type == "TCP" || device_type == "ESP32" || device_type == "ESP8266") {
        if (dev_ip.value == "" || dev_port.value == "") {
            alert("Please fill out all the fields before saving!");
            return false;
        }
    }
    else {
        if (dev_baud.value == "" || dev_data.value == "" || dev_stop.value == "") {
            alert("Please fill out all the fields before saving!");
            return false;
        }
    }
    return confirm("click tp submot");
    return false;
    return true;
}
