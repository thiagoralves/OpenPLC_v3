from flask import Blueprint, send_file

blueprint = Blueprint("dashboardApi", __name__)


@blueprint.route("/dashboard", methods=["GET", "POST"])
def dashboard():
    return send_file("static/html/dashboard.html")
