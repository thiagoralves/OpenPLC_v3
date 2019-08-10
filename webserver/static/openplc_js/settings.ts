declare var $: any;

var modbus_checkbox = document.getElementById('modbus_enabled') as HTMLInputElement;
var modbus_port = document.getElementById('modbus_port') as HTMLInputElement;
var dnp3_checkbox = document.getElementById('dnp3_enabled') as HTMLInputElement;
var dnp3_port = document.getElementById('dnp3_port') as HTMLInputElement;
var auto_run_checkbox = document.getElementById('auto_run') as HTMLInputElement;
var auto_run_text = document.getElementById('auto_run_text') as HTMLInputElement;


window.onload = function () {
    update_widgets();
}

function update_widgets() {

    if (modbus_checkbox.checked == true) {
        modbus_port.disabled = false;
        modbus_port.focus();
    } else {
        modbus_port.disabled = true;
    }

    if (dnp3_checkbox.checked == true) {
        dnp3_port.disabled = false;
        dnp3_port.focus();
    } else {
        dnp3_port.disabled = true;
    }

    if (auto_run_checkbox.checked == true) {
        auto_run_text.value = 'true';
    }
    else {
        auto_run_text.value = 'false';
    }
}

modbus_checkbox.onchange = function () {
    update_widgets();
}

dnp3_checkbox.onchange = function () {
    update_widgets();
}

document.getElementById('auto_run').onchange = function () {
    update_widgets();
}
