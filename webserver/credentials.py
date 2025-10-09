import os
import re
import subprocess
from ipaddress import ip_address
from pathlib import Path


def validate_hostname(hostname: str) -> str:
    """
    Validate and sanitize hostname for use in certificate generation.

    Ensures the hostname:
    - Is not empty or too long (max 253 characters per RFC 1123)
    - Contains only valid characters (alphanumeric, dots, hyphens)
    - Doesn't contain DN special characters that could cause injection

    Args:
        hostname: The hostname to validate

    Returns:
        The validated hostname

    Raises:
        ValueError: If hostname is invalid
    """
    if not hostname or not isinstance(hostname, str):
        raise ValueError("Hostname must be a non-empty string")

    hostname = hostname.strip()

    if len(hostname) > 253:
        raise ValueError(f"Hostname too long: {len(hostname)} characters (max 253)")

    hostname_pattern = re.compile(
        r'^[A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?(\.[A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?)*$'
    )

    if not hostname_pattern.match(hostname):
        raise ValueError(
            f"Invalid hostname format: '{hostname}'. "
            "Hostname must contain only alphanumeric characters, dots, and hyphens"
        )

    dn_special_chars = set('/=+,<>#;"*\\\n\r\t')
    if any(char in hostname for char in dn_special_chars):
        raise ValueError(
            f"Hostname contains invalid characters. "
            f"DN special characters are not allowed: {dn_special_chars}"
        )

    return hostname


def validate_ip_address(ip: str) -> str:
    """
    Validate IP address for use in certificate SAN.

    Args:
        ip: The IP address string to validate

    Returns:
        The validated IP address as a string

    Raises:
        ValueError: If IP address is invalid
    """
    if not ip or not isinstance(ip, str):
        raise ValueError("IP address must be a non-empty string")

    try:
        ip_obj = ip_address(ip.strip())
        return str(ip_obj)
    except ValueError as e:
        raise ValueError(f"Invalid IP address '{ip}': {e}") from e


def validate_file_path(file_path: str, base_dir: str | None = None) -> Path:
    """
    Validate file path to prevent path traversal attacks.

    Args:
        file_path: The file path to validate
        base_dir: Optional base directory to restrict paths to

    Returns:
        Validated Path object

    Raises:
        ValueError: If path is invalid or contains traversal sequences
    """
    if not file_path or not isinstance(file_path, str):
        raise ValueError("File path must be a non-empty string")

    path = Path(file_path).resolve()

    if base_dir:
        base = Path(base_dir).resolve()
        try:
            path.relative_to(base)
        except ValueError as e:
            raise ValueError(
                f"Path '{file_path}' is outside allowed directory '{base_dir}'"
            ) from e

    return path


class CertGen:
    """Generates a self-signed TLS certificate and private key using OpenSSL CLI."""

    MAX_SAN_ENTRIES = 100

    def __init__(self, hostname, ip_addresses=None):
        """
        Initialize certificate generator with validated inputs.

        Args:
            hostname: The hostname for the certificate CN and DNS SAN
            ip_addresses: Optional list of IP addresses for IP SANs

        Raises:
            ValueError: If inputs are invalid
        """
        self.hostname = validate_hostname(hostname)

        self.ip_addresses = []
        if ip_addresses:
            if not isinstance(ip_addresses, (list, tuple)):
                raise ValueError("ip_addresses must be a list or tuple")

            if len(ip_addresses) > self.MAX_SAN_ENTRIES:
                raise ValueError(
                    f"Too many IP addresses: {len(ip_addresses)} "
                    f"(max {self.MAX_SAN_ENTRIES})"
                )

            for ip in ip_addresses:
                validated_ip = validate_ip_address(ip)
                self.ip_addresses.append(validated_ip)

    def generate_self_signed_cert(self, cert_file="cert.pem", key_file="key.pem"):
        """
        Generate a self-signed certificate using OpenSSL CLI.

        Args:
            cert_file: Path where certificate will be saved
            key_file: Path where private key will be saved

        Returns:
            Success message string

        Raises:
            ValueError: If file paths are invalid
            RuntimeError: If certificate generation fails
        """
        print(f"Generating self-signed certificate for {self.hostname}...")

        cert_path = str(validate_file_path(cert_file))
        key_path = str(validate_file_path(key_file))

        san_list = [f"DNS:{self.hostname}"]
        for ip in self.ip_addresses:
            san_list.append(f"IP:{ip}")

        if len(san_list) > self.MAX_SAN_ENTRIES:
            raise ValueError(
                f"Too many SAN entries: {len(san_list)} (max {self.MAX_SAN_ENTRIES})"
            )

        san_string = ",".join(san_list)

        cmd = [
            "openssl",
            "req",
            "-x509",
            "-newkey",
            "rsa:4096",
            "-sha256",
            "-nodes",
            "-keyout",
            key_path,
            "-out",
            cert_path,
            "-days",
            "36500",
            "-subj",
            f"/CN={self.hostname}",
            "-addext",
            f"subjectAltName={san_string}",
        ]

        try:
            subprocess.run(cmd, check=True, capture_output=True, text=True)
            print(f"Certificate saved to {cert_path}")
            print(f"Private key saved to {key_path}")
            return f"Certificate generated successfully for {self.hostname}"
        except subprocess.CalledProcessError as e:
            error_msg = f"Error generating certificate: {e.stderr}"
            print(error_msg)
            raise RuntimeError(error_msg) from e
        except FileNotFoundError as exc:
            error_msg = "OpenSSL not found. Please ensure OpenSSL is installed."
            print(error_msg)
            raise RuntimeError(error_msg) from exc

    def is_certificate_valid(self, cert_file):
        """
        Check if the certificate exists and is not expired using OpenSSL.

        Args:
            cert_file: Path to the certificate file

        Returns:
            True if certificate is valid, False otherwise
        """
        try:
            cert_path = str(validate_file_path(cert_file))
        except ValueError as e:
            print(f"Invalid certificate path: {e}")
            return False

        if not os.path.exists(cert_path):
            print(f"Certificate file not found: {cert_path}")
            return False

        try:
            result = subprocess.run(
                [
                    "openssl",
                    "x509",
                    "-in",
                    cert_path,
                    "-noout",
                    "-checkend",
                    "0",
                ],
                check=False,
                capture_output=True,
                text=True,
            )

            if result.returncode == 0:
                date_result = subprocess.run(
                    [
                        "openssl",
                        "x509",
                        "-in",
                        cert_path,
                        "-noout",
                        "-enddate",
                    ],
                    check=True,
                    capture_output=True,
                    text=True,
                )
                expiry_line = date_result.stdout.strip()
                print(f"Certificate is valid. {expiry_line}")
                return True
            print("Certificate has expired.")
            return False

        except subprocess.CalledProcessError as e:
            print(f"Error checking certificate validity: {e.stderr}")
            return False
        except FileNotFoundError:
            print("OpenSSL not found. Please ensure OpenSSL is installed.")
            return False
