var myCodeMirror = CodeMirror.fromTextArea(custom_layer_code, {
  mode: {
    name: "python",
    version: 2,
    singleLineStringErrors: false,
  },
  lineNumbers: true,
  styleActiveLine: true,
  matchBrackets: true,
  theme: "liquibyte",
  indentUnit: 4,
  indentWithTabs: false,
  tabSize: 4,
  extraKeys: {
    Tab: function (cm) {
      cm.replaceSelection("    ", "end");
    },
  },
});

myCodeMirror.setSize(null, 450);

function populateDropdown() {
  fetch("/hardware/hardwareTypes")
    .then((response) => {
      return response.json();
    })
    .then((jsondata) => {
      var e = document.getElementById("hardware_layer");
      var options = "";
      for (let hardware of jsondata) {
        options += `<option `;
        if (hardware.isSelected) {
          options += `selected="selected" `;
        }
        options += `value="${hardware.value}">${hardware.label}</option>`;
      }
      e.innerHTML = options;
    });
}

populateDropdown();

fetch("/hardware/python/exampleCode")
  .then((response) => {
    return response.text();
  })
  .then((text) => {
    myCodeMirror.setValue(text);
  });

//Insert a dummy script to load something from the server periodically so that the user cookie won't expire
function loadData() {
  refreshSelector();
  url = "runtime_logs";
  try {
    req = new XMLHttpRequest();
  } catch (e) {
    try {
      req = new ActiveXObject("Msxml2.XMLHTTP");
    } catch (e) {
      try {
        req = new ActiveXObject("Microsoft.XMLHTTP");
      } catch (oc) {
        alert("No AJAX Support");
        return;
      }
    }
  }

  req.onreadystatechange = processReqChange;
  req.open("GET", url, true);
  req.send(null);
}

function processReqChange() {
  //If req shows 'complete'
  if (req.readyState == 4) {
    timeoutID = setTimeout("loadData()", 10000);
  }
}

function refreshSelector() {
  var drop_down = document.getElementById("hardware_layer");
  var psm_div = document.getElementById("psm_code");
  var restore_button = document.getElementById("code_restore");
  if (drop_down.value == "psm_linux" || drop_down.value == "psm_win") {
    restore_button.style.display = "inline";
    psm_div.style.visibility = "visible";
  } else {
    restore_button.style.display = "none";
    psm_div.style.visibility = "hidden";
  }
}
