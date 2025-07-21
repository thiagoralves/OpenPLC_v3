from dotenv import load_dotenv
import os

load_dotenv()  # Loads variables from .env into os.environ

BASE_DIR = os.path.abspath(os.path.dirname(__file__))

class Config:
    SQLALCHEMY_DATABASE_URI = os.getenv("SQLALCHEMY_DATABASE_URI")
    JWT_SECRET_KEY = os.getenv("JWT_SECRET_KEY")
    PEPPER = os.getenv("PEPPER")

class DevConfig(Config):
    SQLALCHEMY_TRACK_MODIFICATIONS = False  # keep performance parity with prod
    DEBUG = True

class ProdConfig(Config):
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    DEBUG = False
    ENV = "production"
