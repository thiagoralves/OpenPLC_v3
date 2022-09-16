(function (global) {
  if (typeof global === "undefined") {
    throw new Error("window is undefined");
  }

  var _hash = "!";
  var noBackPlease = function () {
    global.location.href += "#";

    // making sure we have the fruit available for juice....
    // 50 milliseconds for just once do not cost much (^__^)
    global.setTimeout(function () {
      global.location.href += "!";
    }, 50);
  };

  // Earlier we had setInerval here....
  global.onhashchange = function () {
    if (global.location.hash !== _hash) {
      global.location.hash = _hash;
    }
  };

  global.onload = function () {
    loadData();
    noBackPlease();

    // disables backspace on page except on input fields and textarea..
    document.body.onkeydown = function (e) {
      var elm = e.target.nodeName.toLowerCase();
      if (e.which === 8 && elm !== "input" && elm !== "textarea") {
        e.preventDefault();
      }
      // stopping event bubbling up the DOM tree..
      e.stopPropagation();
    };
  };
})(window);

var req;

function loadData() {
  url = "compilation-logs";
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
    compilation_logs = document.getElementById("compilationLog");
    dashboardButton = document.getElementById("dashboardButton");

    //If 'OK'
    if (req.status == 200) {
      //Update textarea text
      compilation_logs.value = req.responseText;
      if (
        req.responseText.search("Compilation finished with errors!") != -1 ||
        req.responseText.search("Compilation finished successfully!") != -1
      ) {
        dashboardButton.style.background = "#E02222";
        dashboardButton.style.pointerEvents = "auto";
      }

      //Start a new update timer
      timeoutID = setTimeout("loadData()", 1000);
    } else {
      compilation_logs.value =
        "There was a problem retrieving the logs. Error: " + req.statusText;
    }
  }
}

function validateForm() {
  var progname = document.forms["uploadForm"]["prog_name"].value;
  if (progname == "") {
    alert("Program Name cannot be blank");
    return false;
  }
  return true;
}

function getInfo() {
  // window.url()
  // pegar os parametros de get
  // enviar requisição para url de get info
  // popular
}

function submitFile() {
  var file = document.forms["uploadForm"]["file"].value;
  if (file == "") {
    popError("No file selected");
    return false;
  }
  return true;
}
