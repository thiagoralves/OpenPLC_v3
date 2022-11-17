from flask import Blueprint, send_file, redirect, request
from flask_login import current_user
from database.login import validateLogin
from .utils import createRedirectUrl
import json

blueprint = Blueprint("loginApi", __name__)


@blueprint.route("/login", methods=["GET", "POST"])
def login():
    if current_user.is_authenticated:
        return redirect('/dashboard', 302)
    return send_file("static/html/login.html")


@blueprint.route("/validateLogin", methods=["GET", "POST"])
def validate():
    if request.method == "POST":
        valid = validateLogin(
            request.form.get("username", ""), request.form.get("password", "")
        )
    if not valid:
        message = {"error": "Invalid username and password"}
        url = createRedirectUrl("login", message)
        return redirect(url, code=302)

    return redirect("/dashboard", code=302)
