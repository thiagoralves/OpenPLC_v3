var mytext = document.getElementById("mytextarea");
mytext.scrollTop = document.getElementById("mytextarea").scrollHeight;
var req;

function copyClipboard() {
  var copyText = document.getElementById("mytextarea");
  copyText.select();
  document.execCommand("Copy");

  var tooltip = document.getElementById("myTooltip");
  tooltip.innerHTML = "Copied!";
}

function outFunc() {
  var tooltip = document.getElementById("myTooltip");
  tooltip.innerHTML = "Copy to clipboard";
}

function loadData() {
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
    runtime_logs = document.getElementById("mytextarea");

    //If 'OK'
    if (req.status == 200) {
      //Update textarea text
      runtime_logs.value = req.responseText;

      //Start a new update timer
      timeoutID = setTimeout("loadData()", 1000);
    } else {
      runtime_logs.value =
        "There was a problem retrieving the logs. Error: " + req.statusText;
    }
  }
}
