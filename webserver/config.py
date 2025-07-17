import os

BASE_DIR = os.path.abspath(os.path.dirname(__file__))

class Config:
    # SQLite DB in project root
    SQLALCHEMY_DATABASE_URI = f"sqlite:///{os.path.join(BASE_DIR, 'restapi.db')}"
    JWT_SECRET_KEY = "openplc"  # TODO change password

class DevConfig(Config):
    SQLALCHEMY_TRACK_MODIFICATIONS = True
    DEBUG = True

class ProdConfig(Config):
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    DEBUG = False
