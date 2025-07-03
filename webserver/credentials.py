from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.backends import default_backend
import datetime
import ipaddress
import os

# key = rsa.generate_private_key(
#     public_exponent=65537,
#     key_size=4096,
#     backend=default_backend()
# )

# with open("/home/lucas/Documents/secrets/key.pem", "wb") as f:
#      f.write(key.private_bytes(
#         encoding=serialization.Encoding.PEM,
#         format=serialization.PrivateFormat.TraditionalOpenSSL,
#         encryption_algorithm=serialization.BestAvailableEncryption(b"passphrase"),
#      ))

# csr = x509.CertificateSigningRequestBuilder().subject_name(x509.Name([
#      # Provide various details about who we are.
#      x509.NameAttribute(NameOID.COUNTRY_NAME, "US"),
#      x509.NameAttribute(NameOID.STATE_OR_PROVINCE_NAME, "California"),
#      x509.NameAttribute(NameOID.LOCALITY_NAME, "San Francisco"),
#      x509.NameAttribute(NameOID.ORGANIZATION_NAME, "My Company"),
#      x509.NameAttribute(NameOID.COMMON_NAME, "mysite.com"),
#      ])).add_extension(
#      x509.SubjectAlternativeName([
#          # Describe what sites we want this certificate for.
#          x509.DNSName("mysite.com"),
#          x509.DNSName("www.mysite.com"),
#          x509.DNSName("subdomain.mysite.com"),
#      ]),
#      critical=False,
#      # Sign the CSR with our private key.
# ).sign(key, hashes.SHA256())

# # Write our CSR out to disk.
# with open("/home/lucas/Documents/secrets/csr.pem", "wb") as f:
#     f.write(csr.public_bytes(serialization.Encoding.PEM))

def generate_self_signed_cert(hostname, ip_addresses=None, cert_file="cert.pem", key_file="key.pem"):
    """
    Generates a self-signed TLS certificate and private key.

    Args:
        hostname (str): The common name (CN) for the certificate.
        ip_addresses (list, optional): A list of IP addresses to include in the SAN extension.
        cert_file (str): The filename for the certificate (PEM format).
        key_file (str): The filename for the private key (PEM format).
    """
    print(f"Generating self-signed certificate for {hostname}...")

    # Generate our key
    key = rsa.generate_private_key(
        public_exponent=65537,
        key_size=2048,
        backend=default_backend()
    )

    # Subject and Issuer
    subject = issuer = x509.Name([
        x509.NameAttribute(NameOID.COUNTRY_NAME, u"US"),
        x509.NameAttribute(NameOID.STATE_OR_PROVINCE_NAME, u"California"),
        x509.NameAttribute(NameOID.LOCALITY_NAME, u"San Francisco"),
        x509.NameAttribute(NameOID.ORGANIZATION_NAME, u"My Development Company"),
        x509.NameAttribute(NameOID.COMMON_NAME, hostname),
    ])

    # Subject Alternative Names (SAN)
    alt_names = [x509.DNSName(hostname)]
    if ip_addresses:
        for addr in ip_addresses:
            alt_names.append(x509.IPAddress(ipaddress.ip_address(addr)))

    san_extension = x509.SubjectAlternativeName(alt_names)

    # Certificate validity
    now = datetime.datetime.utcnow()
    cert = (
        x509.CertificateBuilder()
        .subject_name(subject)
        .issuer_name(issuer)
        .public_key(key.public_key())
        .serial_number(x509.random_serial_number())
        .not_valid_before(now)
        .not_valid_after(now + datetime.timedelta(days=365))  # Valid for 1 year
        .add_extension(x509.BasicConstraints(ca=True, path_length=None), critical=True)
        .add_extension(san_extension, critical=False)
        .sign(key, hashes.SHA256(), default_backend())
    )

    # Write our certificate and key to disk
    with open(cert_file, "wb") as f:
        f.write(cert.public_bytes(serialization.Encoding.PEM))
    with open(key_file, "wb") as f:
        f.write(key.private_bytes(
            serialization.Encoding.PEM,
            serialization.PrivateFormat.PKCS8,
            serialization.NoEncryption()
        ))
    print(f"Certificate saved to {cert_file}")
    print(f"Private key saved to {key_file}")

if __name__ == '__main__':
    CERT_FILE = "secrets/cert.pem"
    KEY_FILE = "secrets/key.pem"
    HOSTNAME = "localhost" # Or your local IP address, e.g., "127.0.0.1"

    # Generate certificate if it doesn't exist
    if not os.path.exists(CERT_FILE) or not os.path.exists(KEY_FILE):
        generate_self_signed_cert(HOSTNAME, ip_addresses=["127.0.0.1"], cert_file=CERT_FILE, key_file=KEY_FILE)
