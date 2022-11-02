function onCloseError() {
  document.getElementById("errorBackdrop").remove();
}

function popError(errorMessage) {
  const errorBackdrop = document.createElement("div");
  errorBackdrop.id = "errorBackdrop";

  const errorPanel = document.createElement("div");
  errorPanel.className = "errorPanel";

  const errorTitle = document.createElement("h2");
  errorTitle.className = "errorTitle";
  errorTitle.innerText = "Error";

  const errImgWrapper = document.createElement("div");
  errImgWrapper.className = "errImgWrapper";

  const errorImg = document.createElement("img");
  errorImg.id = "errorImg";
  errorImg.src = "static/assets/error.png";

  const errorContent = document.createElement("div");
  errorContent.className = "errorContent";
  errorContent.innerText = errorMessage;

  const closeError = document.createElement("button");
  closeError.className = "button closeError";
  closeError.onclick = onCloseError;
  closeError.innerText = "Close";

  errImgWrapper.appendChild(errorImg);
  errorPanel.appendChild(errorTitle);
  errorPanel.appendChild(errImgWrapper);
  errorPanel.appendChild(errorContent);
  errorPanel.appendChild(closeError);
  errorBackdrop.appendChild(errorPanel);

  document.body.appendChild(errorBackdrop);
}

function findGetParameter(parameterName) {
  var result = null,
    tmp = [];
  var items = location.search.substr(1).split("&");
  for (var index = 0; index < items.length; index++) {
    tmp = items[index].split("=");
    if (tmp[0] === parameterName) result = decodeURIComponent(tmp[1]);
  }
  return result;
}

window.onload = () => {
  error = findGetParameter("error");
  if (error) {
    popError(error);
  }
};
