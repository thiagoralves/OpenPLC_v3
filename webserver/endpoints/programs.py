from flask import Blueprint, send_file

blueprint = Blueprint("programsApi", __name__)


@blueprint.route("/programs", methods=["GET", "POST"])
def programs():
    return send_file("static/html/programs.html")


# # Remove this when error workflow implemented
# @blueprint.route("/uploadError", methods=["GET", "POST"])
# def uploadError():
#     with open(
#         "static/html/programs/uploadProgram/errorPage/uploadError.html", "r"
#     ) as htmlfile:
#         return htmlfile.read()


@blueprint.route("/uploadProgram", methods=["GET", "POST"])
def uploadProgram():
    return send_file("static/html/programs/uploadProgram/uploadProgram.html")


@blueprint.route("/reloadProgram", methods=["GET", "POST"])
def reloadProgram():
    return send_file("static/html/programs/reloadProgram/reloadProgram.html")


@blueprint.route("/updateProgram", methods=["GET", "POST"])
def updateProgram():
    return send_file("static/html/programs/updateProgram/updateProgram.html")


@blueprint.route("/compileProgram", methods=["GET", "POST"])
def compileProgram():
    return send_file("static/html/programs/compileProgram/compileProgram.html")
