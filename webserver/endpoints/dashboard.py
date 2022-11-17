from flask import Blueprint, send_file
from flask_login import login_required

blueprint = Blueprint("dashboardApi", __name__)


@blueprint.route("/dashboard", methods=["GET", "POST"])
@login_required
def dashboard():
    return send_file("static/html/dashboard.html")
