# Creates User Table
User = "CREATE TABLE User"
User += " (id INTEGER not null,"
User += " name VARCHAR(255) not null,"
User += " username VARCHAR(255) not null,"
User += " email VARCHAR(255) not null,"
User += " password VARCHAR(255) not null,"
User += " profile_picture VARCHAR(255),"
User += " salt VARCHAR(255),"
User += " CONSTRAINT pk_user PRIMARY KEY (id),"
User += " CONSTRAINT uq_user UNIQUE (username));"

# Creates Settings table
Settings = "CREATE TABLE Settings"
Settings += " (key VARCHAR(20) not null,"
Settings += " value VARCHAR(20) not null,"
Settings += " CONSTRAINT pk_settings PRIMARY KEY (key));"

allTables = [User, Settings]
