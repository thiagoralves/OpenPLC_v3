from dotenv import load_dotenv
import os

from pathlib import Path
from dotenv import load_dotenv

# Always resolve .env relative to the repo root to guarantee it is found
ENV_PATH = Path(__file__).resolve().parent.parent / ".env"
load_dotenv(dotenv_path=ENV_PATH, override=False)
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
