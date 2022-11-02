from flask import Blueprint, send_file

blueprint = Blueprint("monitoringApi", __name__)


@blueprint.route("/monitoring", methods=["GET", "POST"])
def monitoring():
    return send_file("static/html/monitoring.html")
