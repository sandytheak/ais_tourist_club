-- initial creation of DB
-- PostgreSQL is chosen for the project

-- create database if missing
CREATE DATABASE tourist_club_db;

-- CREATE TABLES
-- Entity: Users
CREATE TABLE users (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL, -- for bcrypt/argon2 it is better 255 symbols
    email VARCHAR(100) NOT NULL UNIQUE,
    role VARCHAR(20) NOT NULL CHECK (role IN ('TOURIST', 'AGENCY', 'ADMINISTRATION', 'TECH_SUPPORT')),

    -- additional columns for Tourist, Administation, TechAdmin
    first_name VARCHAR(50),
    second_name VARCHAR(50),

    -- additional columns for Tourist
    phone VARCHAR(20),

    -- for agency representatives
    agency_id BIGINT REFERENCES agencies(id) ON DELETE SET NULL
);

-- Entity: Agency
CREATE TABLE agency (
	id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
	name VARCHAR(50),
	registration_date DATE NOT NULL,
	expirience_year INT,
	is_verified BOOLEAN DEFAULT FALSE,
	rate INT NOT NULL CHECK (rate IN (1, 2, 3, 4, 5))
);

-- Entity: Country
CREATE TABLE country (
	id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
	name VARCHAR(50) NOT NULL,
	description VARCHAR(500) NOT NULL,
	visa_required BOOLEAN DEFAULT TRUE
);

-- Entity: Tour
CREATE TABLE tour (
	id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
	country_id BIGINT REFERENCES country(id) ON DELETE CASCADE,
	agency_id BIGINT REFERENCES agencies(id) ON DELETE CASCADE,
	title VARCHAR(80) NOT NULL,
	price DECIMAL(10,2) NOT NULL,
	start_date DATE NOT NULL,
	end_date DATE NOT NULL,
	total_slots INT DEFAULT 1,
	booking_slots INT DEFAULT 0,
	theme VARCHAR(80),
	description TEXT
);

-- Entity: Booking
CREATE TABLE booking (
	id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
	tour_id BIGINT REFERENCES tour(id) ON DELETE CASCADE,
	tourist_id BIGINT REFERENCES users(id) ON DELETE CASCADE,
	booking_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
	status VARCHAR(20) DEFAULT 'Reserved' CHECK (status IN ('Reserved', 'Verified', 'Paid', 'Booked', 'Archived'))
);

-- Entity: SystemLog
CREATE TABLE system_log (
	id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
	date_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
	user_id BIGINT REFERENCES users(id),
	log_level VARCHAR(50),
	message TEXT
);

-- CREATE ROLES
-- Create role for code execution
CREATE ROLE tourism_user_app WITH LOGIN PASSWORD 'tourist';

-- right to connect to dtb and use schema public
GRANT CONNECT ON DATABASE tourist_club_db TO tourism_app_user;
GRANT USAGE ON SCHEMA public TO tourism_app_user;

-- Right to update all tables
GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA public TO tourism_app_user;

-- !!!Right for use of sequences (for GENERATED ALWAYS AS IDENTITY)!!!
-- Without that app can not create new id for tour or users
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO tourism_app_user;



-- CREATE DEFAULT ADMIN
INSERT INTO users (username, password_hash, email, role) 
VALUES ('admin', 'temporary_plain_text_hash', 'admin@touristclub.com', 'ADMIN');