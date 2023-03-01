# -*- coding: utf-8 -*-
from flask import Flask, redirect
from database.login import loginManager
from endpoints import blueprint as runtimeApp
from os import urandom

app = Flask(__name__)
app.secret_key = urandom(12).hex()
appPort = 2346
serverURL = f"http://localhost:{appPort}"
app.register_blueprint(runtimeApp)
loginManager.init_app(app)


@app.route("/", methods=["GET", "POST"])
def root():
    return redirect("/dashboard", 302)


def main():
    print("\033[93m" + "Webserver is starting..." + "\033[0m")


if __name__ == "__main__":
    main()
    app.run(debug=False, host="0.0.0.0", threaded=True, port=appPort)
