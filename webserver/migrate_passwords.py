#!/usr/bin/env python3
"""
Migration script to hash existing plaintext passwords in openplc.db
Run this once to migrate existing users to hashed passwords.
"""

import sqlite3
from sqlite3 import Error
from werkzeug.security import generate_password_hash

# Password hashing configuration (PBKDF2 with SHA256 and 600,000 iterations)
PASSWORD_HASH_METHOD = "pbkdf2:sha256:600000"

def migrate_passwords(db_path="openplc.db"):
    """Migrate all plaintext passwords to hashed passwords."""
    try:
        conn = sqlite3.connect(db_path)
        cur = conn.cursor()
        
        # Get all users
        cur.execute("SELECT user_id, username, password FROM Users")
        users = cur.fetchall()
        
        migrated = 0
        for user_id, username, password in users:
            # Check if password is already hashed (hashed passwords start with method identifier)
            if password.startswith("pbkdf2:") or password.startswith("scrypt:"):
                print(f"User '{username}' already has hashed password, skipping.")
                continue
            
            # Hash the plaintext password
            hashed = generate_password_hash(password, method=PASSWORD_HASH_METHOD)
            cur.execute("UPDATE Users SET password = ? WHERE user_id = ?", (hashed, user_id))
            print(f"Migrated password for user '{username}'")
            migrated += 1
        
        conn.commit()
        cur.close()
        conn.close()
        
        print(f"\nMigration complete. {migrated} password(s) migrated.")
        return True
        
    except Error as e:
        print(f"Database error: {e}")
        return False

if __name__ == "__main__":
    migrate_passwords()
