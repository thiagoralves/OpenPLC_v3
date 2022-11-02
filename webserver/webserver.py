# -*- coding: utf-8 -*-
from flask import Flask, jsonify, redirect, request
from endpoints import blueprint as runtimeApp
import json

app = Flask(__name__)
appPort = 2346
serverURL = f"http://localhost:{appPort}"
app.register_blueprint(runtimeApp)


@app.route("/", methods=["GET", "POST"])
def root():
    # Check if user is logged in
    return redirect("/login", 302)


def main():
    print("\033[93m" + "Webserver is starting..." + "\033[0m")


if __name__ == "__main__":
    main()
    app.run(debug=False, host="0.0.0.0", threaded=True, port=appPort)
