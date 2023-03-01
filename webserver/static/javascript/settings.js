var checkboxes = {
  modbus_port: "modbus_server",
  dnp3_port: "dnp3_server",
  enip_port: "enip_server",
  pstorage_polling: "pstorage_thread",
  start_run_mode: "auto_run"
}

function populateFields() {
  fetch("/settings?data=true")
    .then((response) => {
      return response.json();
    })
    .then((jsondata) => {
      for (let key in jsondata) {
        var e = document.getElementsByName(key)[0];
        var c = document.getElementById(checkboxes[key]);
        if (key === "start_run_mode") {
          c.checked = jsondata[key] === "true"
          continue
        }
        if (jsondata[key] === "disabled") {
          if (c) c.checked = false;
        } else {
          if (c) c.checked = true;
          e.value = jsondata[key];
        }
      }
      setupCheckboxes()
    });
}

function onLoad() {
  populateFields();
  setupCheckboxes();
  document.getElementById("modbus_server").onchange = function () {
    setupCheckboxes();
  };

  document.getElementById("dnp3_server").onchange = function () {
    setupCheckboxes();
  };

  document.getElementById("enip_server").onchange = function () {
    setupCheckboxes();
  };

  document.getElementById("pstorage_thread").onchange = function () {
    setupCheckboxes();
  };

  document.getElementById("auto_run").onchange = function () {
    setupCheckboxes();
  };
};

function setupCheckboxes() {
  var modbus_checkbox = document.getElementById("modbus_server");
  var modbus_text = document.getElementById("modbus_server_port");
  var dnp3_checkbox = document.getElementById("dnp3_server");
  var dnp3_text = document.getElementById("dnp3_server_port");
  var enip_checkbox = document.getElementById("enip_server");
  var enip_text = document.getElementById("enip_server_port");
  var pstorage_checkbox = document.getElementById("pstorage_thread");
  var pstorage_text = document.getElementById("pstorage_thread_poll");
  var auto_run_checkbox = document.getElementById("auto_run");
  var auto_run_text = document.getElementById("auto_run_text");

  if (modbus_checkbox.checked == true) {
    modbus_text.disabled = false;
  } else {
    modbus_text.disabled = true;
  }

  if (dnp3_checkbox.checked == true) {
    dnp3_text.disabled = false;
  } else {
    dnp3_text.disabled = true;
  }

  if (enip_checkbox.checked == true) {
    enip_text.disabled = false;
  } else {
    enip_text.disabled = true;
  }

  if (pstorage_checkbox.checked == true) {
    pstorage_text.disabled = false;
  } else {
    pstorage_text.disabled = true;
  }

  if (auto_run_checkbox.checked == true) {
    auto_run_text.value = "true";
  } else {
    auto_run_text.value = "false";
  }
}

function validateForm() {
  var modbus_checkbox = document.forms["uploadForm"]["modbus_server"].checked;
  var modbus_port = document.forms["uploadForm"]["modbus_server_port"].value;
  var dnp3_checkbox = document.forms["uploadForm"]["dnp3_server"].checked;
  var dnp3_port = document.forms["uploadForm"]["dnp3_server_port"].value;
  var enip_checkbox = document.forms["uploadForm"]["enip_server"].checked;
  var enip_port = document.forms["uploadForm"]["enip_server_port"].value;
  var pstorage_checkbox =
    document.forms["uploadForm"]["pstorage_thread"].checked;
  var pstorage_poll =
    document.forms["uploadForm"]["pstorage_thread_poll"].value;

  if (
    modbus_checkbox &&
    (Number(modbus_port) < 0 || Number(modbus_port) > 65535)
  ) {
    alert("Please select a port number between 0 and 65535");
    return false;
  }
  if (dnp3_checkbox && (Number(dnp3_port) < 0 || Number(dnp3_port) > 65535)) {
    alert("Please select a port number between 0 and 65535");
    return false;
  }
  if (enip_checkbox && (Number(enip_port) < 0 || Number(enip_port) > 65535)) {
    alert("Please select a port number between 0 and 65535");
    return false;
  }
  if (pstorage_checkbox && Number(pstorage_poll) < 0) {
    alert("Persistent Storage polling rate must be bigger than zero");
    return false;
  }

  return true;
}
