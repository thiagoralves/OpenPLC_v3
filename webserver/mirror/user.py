from .classes import Integer, String

User = {
    "id": Integer,
    "name": String,
    "username": String,
    "password": String,
    "email": String,
    "salt": String,
    "profile_picture": String,
}

UserNullable = ["profile_picture", "id"]
