from dotenv import load_dotenv
import os

from pathlib import Path
from dotenv import load_dotenv

# Always resolve .env relative to the repo root to guarantee it is found
ENV_PATH = Path(__file__).resolve().parent.parent / ".env"
load_dotenv(dotenv_path=ENV_PATH, override=False)
BASE_DIR = os.path.abspath(os.path.dirname(__file__))

class Config:
    # Mandatory settings – raise immediately if not provided
    for _var in ("SQLALCHEMY_DATABASE_URI", "JWT_SECRET_KEY", "PEPPER"):
        if not os.getenv(_var):
            raise RuntimeError(f"Environment variable '{_var}' is required but not set")

    SQLALCHEMY_DATABASE_URI = os.environ["SQLALCHEMY_DATABASE_URI"]
    JWT_SECRET_KEY = os.environ["JWT_SECRET_KEY"]
    PEPPER = os.environ["PEPPER"]
class DevConfig(Config):
    SQLALCHEMY_TRACK_MODIFICATIONS = False  # keep performance parity with prod
    DEBUG = True

class ProdConfig(Config):
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    DEBUG = False
    ENV = "production"
