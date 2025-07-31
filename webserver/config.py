from dotenv import load_dotenv
import os
import re
import secrets
import logging

from pathlib import Path
from dotenv import load_dotenv

# Always resolve .env relative to the repo root to guarantee it is found
ENV_PATH = Path(__file__).resolve().parent.parent / ".env"
DB_PATH = Path(__file__).resolve().parent.parent / "restapi.db"
BASE_DIR = os.path.abspath(os.path.dirname(__file__))

logger = logging.getLogger(__name__)
logging.basicConfig(
    level=logging.DEBUG,  # Minimum level to capture
    format='[%(levelname)s] %(asctime)s - %(message)s',
    datefmt='%H:%M:%S'
)

# Function to validate environment variable values
def is_valid_env(var_name, value):
    if var_name == "SQLALCHEMY_DATABASE_URI":
        return value.startswith("sqlite:///")
    elif var_name in ("JWT_SECRET_KEY", "PEPPER"):
        return bool(re.fullmatch(r"[a-fA-F0-9]{64}", value))
    return False

# Function to generate a new .env file with valid defaults
def generate_env_file():
    jwt = secrets.token_hex(32)
    pepper = secrets.token_hex(32)
    uri = "sqlite:///{DB_PATH}"

    with open(ENV_PATH, "w") as f:
        f.write("FLASK_ENV=development\n")
        f.write(f"SQLALCHEMY_DATABASE_URI={uri}\n")
        f.write(f"JWT_SECRET_KEY={jwt}\n")
        f.write(f"PEPPER={pepper}\n")

    os.chmod(ENV_PATH, 0o600)
    logger.info(f".env file created at {ENV_PATH}")

    # Ensure the database file exists and is writable
    # Deletion is required because new secrets will change the database saved hashes
    if os.path.exists(DB_PATH):
        os.remove(DB_PATH)
        logger.info(f"Deleted existing database file: {DB_PATH}")

# Load .env file
if not os.path.isfile(ENV_PATH):
    logger.warning(".env file not found, creating one...")
    generate_env_file()

load_dotenv(dotenv_path=ENV_PATH, override=False)

# Mandatory settings – raise immediately if not provided
try:
    for var in ("SQLALCHEMY_DATABASE_URI", "JWT_SECRET_KEY", "PEPPER"):
        val = os.getenv(var)
        if not val or not is_valid_env(var, val):
            raise RuntimeError(f"Environment variable '{var}' is invalid or missing")
except RuntimeError as e:
    logger.error(f"{e}")
    # Need to regenerate .env file and remove the database as well
    response = input("Do you want to regenerate the .env file? This will delete your database. [y/N]: ").strip().lower()
    if response == 'y':
        logger.info("Regenerating .env with new valid values...")
        generate_env_file()
        load_dotenv(ENV_PATH)
    else:
        logger.error("Exiting due to invalid environment configuration.")
        exit(1)


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
