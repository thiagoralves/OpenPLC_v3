from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes

key = rsa.generate_private_key(
     public_exponent=65537,
     key_size=2048,
)

with open("/home/lucas/Documents/secrets/key.pem", "wb") as f:
     f.write(key.private_bytes(
         encoding=serialization.Encoding.PEM,
         format=serialization.PrivateFormat.TraditionalOpenSSL,
         encryption_algorithm=serialization.BestAvailableEncryption(b"passphrase"),
     ))

csr = x509.CertificateSigningRequestBuilder().subject_name(x509.Name([
     # Provide various details about who we are.
     x509.NameAttribute(NameOID.COUNTRY_NAME, "US"),
     x509.NameAttribute(NameOID.STATE_OR_PROVINCE_NAME, "California"),
     x509.NameAttribute(NameOID.LOCALITY_NAME, "San Francisco"),
     x509.NameAttribute(NameOID.ORGANIZATION_NAME, "My Company"),
     x509.NameAttribute(NameOID.COMMON_NAME, "mysite.com"),
     ])).add_extension(
     x509.SubjectAlternativeName([
         # Describe what sites we want this certificate for.
         x509.DNSName("mysite.com"),
         x509.DNSName("www.mysite.com"),
         x509.DNSName("subdomain.mysite.com"),
     ]),
     critical=False,
     # Sign the CSR with our private key.
).sign(key, hashes.SHA256())

# Write our CSR out to disk.
with open("/home/lucas/Documents/secrets/csr.pem", "wb") as f:
    f.write(csr.public_bytes(serialization.Encoding.PEM))