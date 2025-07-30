from dotenv import load_dotenv
import os
import re
import secrets
import logging

from pathlib import Path
from dotenv import load_dotenv

# Always resolve .env relative to the repo root to guarantee it is found
ENV_PATH = Path(__file__).resolve().parent.parent / ".env"
BASE_DIR = os.path.abspath(os.path.dirname(__file__))
logger = logging.getLogger(__name__)

# Function to validate environment variable values
def is_valid_env(var_name, value):
    if var_name == "SQLALCHEMY_DATABASE_URI":
        return value.startswith("sqlite:///") or value.startswith("postgresql://") or value.startswith("mysql://")
    elif var_name in ("JWT_SECRET_KEY", "PEPPER"):
        return bool(re.fullmatch(r"[a-fA-F0-9]{64}", value))
    return False

# Function to generate a new .env file with valid defaults
def generate_env_file():
    jwt = secrets.token_hex(32)
    pepper = secrets.token_hex(32)
    uri = "sqlite:///restapi.db"

    with open(ENV_PATH, "w") as f:
        f.write("FLASK_ENV=development\n")
        f.write(f"SQLALCHEMY_DATABASE_URI={uri}\n")
        f.write(f"JWT_SECRET_KEY={jwt}\n")
        f.write(f"PEPPER={pepper}\n")

    os.chmod(ENV_PATH, 0o600)
    logger.info(f"✅ .env file created at {ENV_PATH}")

# Load .env file
if not os.path.isfile(ENV_PATH):
    logger.warning("⚠️  .env file not found, creating one...")
    generate_env_file()

load_dotenv(dotenv_path=ENV_PATH, override=False)

# Mandatory settings – raise immediately if not provided
try:
    for _var in ("SQLALCHEMY_DATABASE_URI", "JWT_SECRET_KEY", "PEPPER"):
        if not os.getenv(_var):
            raise RuntimeError(f"Environment variable '{_var}' is required but not set")
    for var in ("SQLALCHEMY_DATABASE_URI", "JWT_SECRET_KEY", "PEPPER"):
        val = os.getenv(var)
        if not val or not is_valid_env(var, val):  # <<<<<<<< CALL HAPPENS HERE
            raise RuntimeError(f"Environment variable '{var}' is invalid or missing")
except RuntimeError as e:
    logger.error(f"❌ {e}")
    logger.info("🔁 Regenerating .env with new valid values...")
    generate_env_file()
    load_dotenv(ENV_PATH)

class Config:
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
