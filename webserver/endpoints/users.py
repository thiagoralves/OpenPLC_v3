from flask import Blueprint, send_file, request, redirect
from os.path import splitext, join
from database.user import createUser

blueprint = Blueprint("usersApi", __name__)


@blueprint.route("/users", methods=["GET", "POST"])
def users():
    return send_file("static/html/users.html")


@blueprint.route("/addUser", methods=["GET", "POST"])
def addUser():
    return send_file("static/html/users/addUser/addUser.html")


@blueprint.route("/createUser", methods=["GET", "POST"])
def createNewUser():
    if request.method == "POST":
        file = None
        user = {}
        user["name"] = request.form.get("full_name")
        user["username"] = request.form.get("user_name")
        # Add hash
        user["password"] = request.form.get("user_password")
        user["email"] = request.form.get("user_email")
        if "file" not in request.files:
            user["profile_picture"] = None
        else:
            file = request.files["file"]
            if file.filename == "":
                file = None
                user["profile_picture"] = None
            else:
                filename, extension = splitext(file.filename)
                if extension not in (".jpg", ".jpeg", ".png"):
                    raise Exception("Invalid profile picture file format")
                user["profile_picture"] = str(user["username"]) + extension

        response = createUser(user)
        if file:
            file.save(join("static/profilePictures/", user["profile_picture"]))

        return redirect("/addUser", 302)
