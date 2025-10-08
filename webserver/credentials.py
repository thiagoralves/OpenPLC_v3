import subprocess
import os


class CertGen():
    """Generates a self-signed TLS certificate and private key using OpenSSL CLI."""
    def __init__(self, hostname, ip_addresses=None):
        self.hostname = hostname
        self.ip_addresses = ip_addresses or []

    def generate_self_signed_cert(self, cert_file="cert.pem", key_file="key.pem"):
        """Generate a self-signed certificate using OpenSSL CLI with strong security parameters."""
        print(f"Generating self-signed certificate for {self.hostname}...")
        
        san_list = [f"DNS:{self.hostname}"]
        for ip in self.ip_addresses:
            san_list.append(f"IP:{ip}")
        san_string = ",".join(san_list)
        
        cmd = [
            "openssl", "req",
            "-x509",
            "-newkey", "rsa:4096",
            "-sha256",
            "-nodes",
            "-keyout", str(key_file),
            "-out", str(cert_file),
            "-days", "36500",
            "-subj", f"/CN={self.hostname}",
            "-addext", f"subjectAltName={san_string}"
        ]
        
        try:
            result = subprocess.run(
                cmd,
                check=True,
                capture_output=True,
                text=True
            )
            print(f"Certificate saved to {cert_file}")
            print(f"Private key saved to {key_file}")
            return f"Certificate generated successfully for {self.hostname}"
        except subprocess.CalledProcessError as e:
            error_msg = f"Error generating certificate: {e.stderr}"
            print(error_msg)
            raise RuntimeError(error_msg)
        except FileNotFoundError:
            error_msg = "OpenSSL not found. Please ensure OpenSSL is installed on the system."
            print(error_msg)
            raise RuntimeError(error_msg)

    def is_certificate_valid(self, cert_file):
        """Check if the certificate exists and is not expired using OpenSSL."""
        if not os.path.exists(cert_file):
            print(f"Certificate file not found: {cert_file}")
            return False
        
        try:
            result = subprocess.run(
                ["openssl", "x509", "-in", str(cert_file), "-noout", "-checkend", "0"],
                check=False,
                capture_output=True,
                text=True
            )
            
            if result.returncode == 0:
                date_result = subprocess.run(
                    ["openssl", "x509", "-in", str(cert_file), "-noout", "-enddate"],
                    check=True,
                    capture_output=True,
                    text=True
                )
                expiry_line = date_result.stdout.strip()
                print(f"Certificate is valid. {expiry_line}")
                return True
            else:
                print(f"Certificate has expired.")
                return False
                
        except subprocess.CalledProcessError as e:
            print(f"Error checking certificate validity: {e.stderr}")
            return False
        except FileNotFoundError:
            print("OpenSSL not found. Please ensure OpenSSL is installed on the system.")
            return False
