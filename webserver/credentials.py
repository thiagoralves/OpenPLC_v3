from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.backends import default_backend
import datetime
import ipaddress
import os


class CertGen():
    """
    Generates a self-signed TLS certificate and private key.

    Args:
        hostname (str): The common name (CN) for the certificate.
        ip_addresses (list, optional): A list of IP addresses to include in the SAN extension.
        cert_file (str): The filename for the certificate (PEM format).
        key_file (str): The filename for the private key (PEM format).
    """
    def __init__(self, hostname, ip_addresses=None):
        self.hostname = hostname
        self.ip_addresses = ip_addresses

        # Certificate validity
        self.now = datetime.datetime.utcnow()
        # Subject and Issuer
        self.subject = self.issuer = x509.Name([
            # x509.NameAttribute(NameOID.COUNTRY_NAME, u"US"), # TODO get device country
            # x509.NameAttribute(NameOID.STATE_OR_PROVINCE_NAME, u"openplc"),
            # x509.NameAttribute(NameOID.LOCALITY_NAME, u"openplc"),
            # x509.NameAttribute(NameOID.ORGANIZATION_NAME, u"Autonomy"),
            x509.NameAttribute(NameOID.COMMON_NAME, hostname),
        ])

        # Subject Alternative Names (SAN)
        self.alt_names = [x509.DNSName(hostname)]
        if ip_addresses:
            for addr in ip_addresses:
                self.alt_names.append(x509.IPAddress(ipaddress.ip_address(addr)))

        self.san_extension = x509.SubjectAlternativeName(self.alt_names)
    
    def generate_key(self):
        # Generate our key
        self.key = rsa.generate_private_key(
            public_exponent=65537,
            key_size=2048,
            backend=default_backend()
        )

    def generate_self_signed_cert(self, cert_file="cert.pem", key_file="key.pem"):
        print(f"Generating self-signed certificate for {self.hostname}...")

        self.generate_key()

        cert = (
            x509.CertificateBuilder()
            .subject_name(self.subject)
            .issuer_name(self.issuer)
            .public_key(self.key.public_key())
            .serial_number(x509.random_serial_number())
            .not_valid_before(self.now)
            .not_valid_after(self.now + datetime.timedelta(days=365))  # Valid for 1 year
            .add_extension(x509.BasicConstraints(ca=True, path_length=None), critical=True)
            .add_extension(self.san_extension, critical=False)
            .sign(self.key, hashes.SHA256(), default_backend())
        )

        # Write our certificate and key to disk
        with open(cert_file, "wb+") as f:
            f.write(cert.public_bytes(serialization.Encoding.PEM))
        with open(key_file, "wb+") as f:
            f.write(self.key.private_bytes(
                serialization.Encoding.PEM,
                serialization.PrivateFormat.PKCS8,
                serialization.NoEncryption()
            ))
        print(f"Certificate saved to {cert_file}")
        print(f"Private key saved to {key_file}")

    # TODO add a function to update the certificate on the client before expiration
    def is_certificate_valid(self, cert_file):
        """
        Checks if a certificate is valid (not expired and not yet valid).

        Args:
            cert_file (str): The path to the certificate file (PEM format).

        Returns:
            bool: True if the certificate is currently valid, False otherwise.
        """
        if not os.path.exists(cert_file):
            print(f"Certificate file not found: {cert_file}")
            return False

        try:
            with open(cert_file, "rb") as f:
                cert_data = f.read()
            cert = x509.load_pem_x509_certificate(cert_data, default_backend())

            now = datetime.datetime.utcnow()

            if now < cert.not_valid_before_utc:
                print(f"Certificate is not yet valid. Valid from: {cert.not_valid_before}")
                return False
            if now > cert.not_valid_after_utc:
                print(f"Certificate has expired. Expired on: {cert.not_valid_after}")
                return False

            print(f"Certificate is valid. Expires on: {cert.not_valid_after_utc}")
            return True

        except Exception as e:
            print(f"Error loading or parsing certificate: {e}")
            return False

# if __name__ == '__main__':
#     # TODO define best path to store credentials
#     CERT_FILE = "/etc/ssl/certs/certOPENPLC.pem"
#     KEY_FILE = "/etc/ssl/private/keyOPENPLC.pem"
#     HOSTNAME = "localhost"

#     cert_gen = CertGen(hostname=HOSTNAME, ip_addresses=["127.0.0.1"])
#     # Generate certificate if it doesn't exist
#     if not os.path.exists(CERT_FILE) or not os.path.exists(KEY_FILE):
#         cert_gen.generate_self_signed_cert(cert_file=CERT_FILE, key_file=KEY_FILE)
#     # Verify expiration date
#     elif cert_gen.is_certificate_valid(CERT_FILE):
#         print(cert_gen.generate_self_signed_cert(cert_file=CERT_FILE, key_file=KEY_FILE))
#     # Credentials already created
#     else:
#         print("Credentials already generated!")
