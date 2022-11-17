from flask import Blueprint, send_file
from flask_login import login_required

blueprint = Blueprint("monitoringApi", __name__)


@blueprint.route("/monitoring", methods=["GET", "POST"])
@login_required
def monitoring():
    return send_file("static/html/monitoring.html")
