function populateDropdown() {
  var tcp_stuff = document.getElementById("tcp-stuff");
  var rtu_stuff = document.getElementById("rtu-stuff");
  fetch("/modbus/deviceTypes")
    .then((response) => {
      return response.json();
    })
    .then((jsondata) => {
      var selectedType = "rtu"
      jsondata.forEach(e => {
        if(e.isSelected) {
          selectedType = e.type
        }
      })
      switch(selectedType) {
        case "rtu":
          populateCOM()
          tcp_stuff.style.display = "none"
          rtu_stuff.style.display = "block"
          break
        case "tcp":
          tcp_stuff.style.display = "block"
          rtu_stuff.style.display = "none"
          break
        default:
          break
      }
      var e = document.getElementById("dev_protocol");
      var tcp = document.createElement("optgroup")
      tcp.label = "TCP"
      var rtu = document.createElement("optgroup")
      rtu.label = "RTU"
      for (let hardware of jsondata) {
        var option = document.createElement("option");
        option.value = hardware.value;
        option.selected = hardware.isSelected;
        option.innerHTML = hardware.label;
        if (hardware.type === "tcp") {
          tcp.appendChild(option)
        } else {
          rtu.appendChild(option)
        }
      }
      e.appendChild(rtu);
      e.appendChild(tcp);
    });
}

function populateCOM() {
  fetch("/modbus/comPorts")
    .then((response) => {
      return response.json();
    })
    .then((jsondata) => {
      var e = document.getElementById("dev_cport");
      e.innerHTML = ''
      for (let port of jsondata) {
        var option = document.createElement("option");
        option.value = port.toLowerCase();
        option.innerHTML = port;
        e.appendChild(option)
      }
    });
}

function refreshSelector() {
  var drop_down = document.getElementById("dev_protocol");
  var selected = [...drop_down.options].find(o => o.selected)
  var tcp_stuff = document.getElementById("tcp-stuff");
  var rtu_stuff = document.getElementById("rtu-stuff");
  if (selected.parentElement.label === "TCP") {
    tcp_stuff.style.display = "block"
    rtu_stuff.style.display = "none"
  } else {
    populateCOM()
    rtu_stuff.style.display = "block"
    tcp_stuff.style.display = "none"
  }
}

window.onload = function () {
  populateDropdown();
  setupPageContent();
  LoadValuesFromDB();
};

function turnElementOn(element) {
  element.readOnly = false;
  element.value = "";
  element.style.backgroundColor = "white";
  element.style.color = "black";
}

function turnElementOff(element) {
  element.readOnly = true;
  element.style.backgroundColor = "#F8F8F8";
  element.style.color = "#9C9C9C";
}

function setupPageContent() {
  var dropmenu = document.getElementById("dev_protocol");
  var tcpdiv = document.getElementById("tcp-stuff");
  var rtudiv = document.getElementById("rtu-stuff");

  var devport = document.getElementById("dev_port");
  var devid = document.getElementById("dev_id");

  var devbaud = document.getElementById("dev_baud");
  var devparity = document.getElementById("dev_parity");
  var devdata = document.getElementById("dev_data");
  var devstop = document.getElementById("dev_stop");
  var devpause = document.getElementById("dev_pause");

  var distart = document.getElementById("di_start");
  var disize = document.getElementById("di_size");
  var dostart = document.getElementById("do_start");
  var dosize = document.getElementById("do_size");
  var aistart = document.getElementById("ai_start");
  var aisize = document.getElementById("ai_size");
  var aorstart = document.getElementById("aor_start");
  var aorsize = document.getElementById("aor_size");
  var aowstart = document.getElementById("aow_start");
  var aowsize = document.getElementById("aow_size");

  if (dropmenu.options[dropmenu.selectedIndex].value == "TCP") {
    tcpdiv.style.display = "block";
    rtudiv.style.display = "none";

    turnElementOn(devport);
    turnElementOn(devid);
    turnElementOn(distart);
    turnElementOn(disize);
    turnElementOn(dostart);
    turnElementOn(dosize);
    turnElementOn(aistart);
    turnElementOn(aisize);
    turnElementOn(aorstart);
    turnElementOn(aorsize);
    turnElementOn(aowstart);
    turnElementOn(aowsize);
  } else if (dropmenu.options[dropmenu.selectedIndex].value == "ESP32") {
    tcpdiv.style.display = "block";
    rtudiv.style.display = "none";

    turnElementOff(devport);
    devport.value = "502";
    turnElementOff(devid);
    devid.value = "0";
    turnElementOff(distart);
    distart.value = "0";
    turnElementOff(disize);
    disize.value = "8";
    turnElementOff(dostart);
    dostart.value = "0";
    turnElementOff(dosize);
    dosize.value = "8";
    turnElementOff(aistart);
    aistart.value = "0";
    turnElementOff(aisize);
    aisize.value = "1";
    turnElementOff(aorstart);
    aorstart.value = "0";
    turnElementOff(aorsize);
    aorsize.value = "0";
    turnElementOff(aowstart);
    aowstart.value = "0";
    turnElementOff(aowsize);
    aowsize.value = "1";
  } else if (dropmenu.options[dropmenu.selectedIndex].value == "ESP8266") {
    tcpdiv.style.display = "block";
    rtudiv.style.display = "none";

    turnElementOff(devport);
    devport.value = "502";
    turnElementOff(devid);
    devid.value = "0";
    turnElementOff(distart);
    distart.value = "0";
    turnElementOff(disize);
    disize.value = "8";
    turnElementOff(dostart);
    dostart.value = "0";
    turnElementOff(dosize);
    dosize.value = "8";
    turnElementOff(aistart);
    aistart.value = "0";
    turnElementOff(aisize);
    aisize.value = "1";
    turnElementOff(aorstart);
    aorstart.value = "0";
    turnElementOff(aorsize);
    aorsize.value = "0";
    turnElementOff(aowstart);
    aowstart.value = "0";
    turnElementOff(aowsize);
    aowsize.value = "1";
  } else if (dropmenu.options[dropmenu.selectedIndex].value == "RTU") {
    tcpdiv.style.display = "none";
    rtudiv.style.display = "block";

    turnElementOn(devid);
    turnElementOn(devbaud);
    turnElementOn(devparity);
    devparity.value = "None";
    turnElementOn(devdata);
    turnElementOn(devstop);
    turnElementOn(devpause);
    devpause.value = "0";
    turnElementOn(distart);
    turnElementOn(disize);
    turnElementOn(dostart);
    turnElementOn(dosize);
    turnElementOn(aistart);
    turnElementOn(aisize);
    turnElementOn(aorstart);
    turnElementOn(aorsize);
    turnElementOn(aowstart);
    turnElementOn(aowsize);
  } else if (dropmenu.options[dropmenu.selectedIndex].value == "Uno") {
    tcpdiv.style.display = "none";
    rtudiv.style.display = "block";

    turnElementOff(devid);
    devid.value = "0";
    turnElementOff(devbaud);
    devbaud.value = "115200";
    turnElementOff(devparity);
    devparity.value = "None";
    turnElementOff(devdata);
    devdata.value = "8";
    turnElementOff(devstop);
    devstop.value = "1";
    turnElementOff(devpause);
    devpause.value = "0";
    turnElementOff(distart);
    distart.value = "0";
    turnElementOff(disize);
    disize.value = "5";
    turnElementOff(dostart);
    dostart.value = "0";
    turnElementOff(dosize);
    dosize.value = "4";
    turnElementOff(aistart);
    aistart.value = "0";
    turnElementOff(aisize);
    aisize.value = "6";
    turnElementOff(aorstart);
    aorstart.value = "0";
    turnElementOff(aorsize);
    aorsize.value = "0";
    turnElementOff(aowstart);
    aowstart.value = "0";
    turnElementOff(aowsize);
    aowsize.value = "3";
  } else if (dropmenu.options[dropmenu.selectedIndex].value == "Mega") {
    tcpdiv.style.display = "none";
    rtudiv.style.display = "block";

    turnElementOff(devid);
    devid.value = "0";
    turnElementOff(devbaud);
    devbaud.value = "115200";
    turnElementOff(devparity);
    devparity.value = "None";
    turnElementOff(devdata);
    devdata.value = "8";
    turnElementOff(devstop);
    devstop.value = "1";
    turnElementOff(devpause);
    devpause.value = "0";
    turnElementOff(distart);
    distart.value = "0";
    turnElementOff(disize);
    disize.value = "24";
    turnElementOff(dostart);
    dostart.value = "0";
    turnElementOff(dosize);
    dosize.value = "16";
    turnElementOff(aistart);
    aistart.value = "0";
    turnElementOff(aisize);
    aisize.value = "16";
    turnElementOff(aorstart);
    aorstart.value = "0";
    turnElementOff(aorsize);
    aorsize.value = "0";
    turnElementOff(aowstart);
    aowstart.value = "0";
    turnElementOff(aowsize);
    aowsize.value = "12";
  }
}

document.getElementById("dev_protocol").onchange = function () {
  first_time_edit = false;
  setupPageContent();
};

function validateForm() {
  var devname = document.forms["uploadForm"]["dev_name"].value;
  var devid = document.forms["uploadForm"]["dev_id"].value;

  var devip = document.forms["uploadForm"]["dev_ip"].value;
  var devport = document.forms["uploadForm"]["dev_port"].value;

  var devbaud = document.forms["uploadForm"]["dev_baud"].value;
  var devdata = document.forms["uploadForm"]["dev_data"].value;
  var devstop = document.forms["uploadForm"]["dev_stop"].value;
  var devpause = document.forms["uploadForm"]["dev_pause"].value;

  var distart = document.forms["uploadForm"]["di_start"].value;
  var disize = document.forms["uploadForm"]["di_size"].value;
  var dostart = document.forms["uploadForm"]["do_start"].value;
  var dosize = document.forms["uploadForm"]["do_size"].value;
  var aistart = document.forms["uploadForm"]["ai_start"].value;
  var aisize = document.forms["uploadForm"]["ai_size"].value;
  var aorstart = document.forms["uploadForm"]["aor_start"].value;
  var aorsize = document.forms["uploadForm"]["aor_size"].value;
  var aowstart = document.forms["uploadForm"]["aow_start"].value;
  var aowsize = document.forms["uploadForm"]["aow_size"].value;

  if (
    devname == "" ||
    devid == "" ||
    distart == "" ||
    disize == "" ||
    dostart == "" ||
    dosize == "" ||
    aistart == "" ||
    aisize == "" ||
    aorstart == "" ||
    aorsize == "" ||
    aowstart == "" ||
    aowsize == ""
  ) {
    alert("Please fill out all the fields before saving!");
    return false;
  }

  var dropmenu = document.getElementById("dev_protocol");
  var device_type = dropmenu.options[dropmenu.selectedIndex].value;
  if (
    device_type == "TCP" ||
    device_type == "ESP32" ||
    device_type == "ESP8266"
  ) {
    if (devip == "" || devport == "") {
      alert("Please fill out all the fields before saving!");
      return false;
    }
  } else {
    if (devbaud == "" || devdata == "" || devstop == "") {
      alert("Please fill out all the fields before saving!");
      return false;
    }
  }

  return true;
}
