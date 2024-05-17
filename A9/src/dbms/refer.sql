

-- CREATE TABLE user(
--   U_id INT AUTO_INCREMENT,
--   name VARCHAR(100) NOT NULL,
--   phone_number char(20) NOT NULL,
--   email VARCHAR(100) NOT NULL,
--   u_role VARCHAR(20) NOT NULL,
--   password VARCHAR(100),
--   PRIMARY KEY (U_id)
-- );

-- CREATE TABLE student (
--   S_dept VARCHAR(100) NOT NULL,
--   S_roll VARCHAR(20) NOT NULL,
--   S_id VARCHAR(20) NOT NULL,
--   FOREIGN KEY (S_id) REFERENCES user(U_id),
--   PRIMARY KEY S_id
-- );

-- CREATE TABLE organizer(
--   O_id char(20) NOT NULL,
--   O_position VARCHAR(100) NOT NULL,
--   O_responsibility VARCHAR(200) NOT NULL,
--   FOREIGN KEY (O_id) REFERENCES user(U_id),
--   PRIMARY KEY O_id
-- );

-- CREATE TABLE accomodation(
--   A_id char(20) NOT NULL,
--   A_location VARCHAR(100) NOT NULL,
--   A_capacity INT NOT NULL,
--   PRIMARY KEY A_id
-- );

-- CREATE TABLE mess(
--   M_id CHAR(20) NOT NULL,
--   M_location VARCHAR(100) NOT NULL,
--   M_capacity INT NOT NULL,
--   PRIMARY KEY M_id
-- );

-- CREATE TABLE external_participant(
--   E_id CHAR(20) NOT NULL,
--   E_accID CHAR(20) NOT NULL,
--   E_messID CHAR(20) NOT NULL,
--   E_university char(100) NOT NULL,
--   FOREIGN KEY (E_id) REFERENCES user(U_id),
--   FOREIGN KEY (E_accID) REFERENCES accomodation(A_id),
--   FOREIGN KEY (E_messID) REFERENCES mess(M_id), 
--   PRIMARY KEY E_id
-- );

-- CREATE TABLE venue(
--   V_name CHAR(100) NOT NULL,
--   V_location VARCHAR(200) NOT NULL,
--   V_capacity INT,
--   PRIMARY KEY V_name
-- );

-- CREATE TABLE event (
-- 	E_ID CHAR(100) NOT NULL,
--   E_name CHAR(200) NOT NULL,
--   E_type VARCHAR(100) NOT NULL,
--   E_desc VARCHAR(255) NOT NULL,
--   E_date date NOT NULL,
--   E_time time NOT NULL,
--   E_duration time NOT NULL,
--   E_venue VARCHAR(100) NOT NULL,
--   E_curcnt INT DEFAULT 0,
--   FOREIGN KEY (E_venue) REFERENCES venue(V_name),
--   PRIMARY KEY (E_ID)
-- );

-- CREATE TABLE Registration(
--   R_event CHAR(200) NOT NULL,
--   R_user INT NOT NULL,
--   R_time time NOT NULL,
--   FOREIGN KEY (R_event) REFERENCES event(E_name),
--   FOREIGN KEY (R_user) REFERENCES user(U_id),
--   PRIMARY KEY (R_event, R_user)
-- );

-- CREATE TABLE Volunteer (
--   V_id CHAR(20) NOT NULL,
--   V_event CHAR(200) NOT NULL,
--   FOREIGN KEY (V_id) REFERENCES student (S_id),
--   FOREIGN KEY (V_event) REFERENCES event (E_name),
--   PRIMARY KEY (V_id,V_event)
-- );

-- CREATE TABLE Manage (
--   M_roll CHAR(20) NOT NULL,
--   M_event CHAR(200) NOT NULL,
--   FOREIGN KEY (M_roll) REFERENCES organizer (O_roll),
--   FOREIGN KEY (M_event) REFERENCES event (E_name),
--   PRIMARY KEY (M_roll, M_event)
-- );

-- CREATE TABLE competition (--isa event
--   C_name CHAR(200) NOT NULL,
--   C_props VARCHAR(255) NOT NULL,
--   FOREIGN KEY (C_name) REFERENCES event (E_name),
--   PRIMARY KEY (C_name)
-- );

-- CREATE TABLE prize (
--   P_amount INT NOT NULL,
--   P_position INT NOT NULL,
--   P_userid INT NOT NULL,
--   P_event CHAR(200) NOT NULL,
--   FOREIGN KEY (P_userid) REFERENCES user(U_id),
--   FOREIGN KEY (P_event) REFERENCES event(E_name),
--   PRIMARY KEY (P_event, P_position)
-- );

-- CREATE TABLE sponsor (
--   S_name CHAR(200) NOT NULL,
--   S_desc VARCHAR(200) NOT NULL,
--   PRIMARY KEY (S_name)
-- );

-- CREATE TABLE sponsored_by (
--   SB_type char(20) NOT NULL,
--   SB_contribution INT NOT NULL,
--   SB_sponsor CHAR(200) NOT NULL,
--   SB_event CHAR(200) NOT NULL,
--   FOREIGN KEY (SB_sponsor) REFERENCES sponsor(S_name),
--   FOREIGN KEY (SB_event) REFERENCES event(E_name),
--   PRIMARY KEY (SB_sponsor, SE_event)
-- );







------------------------- UPDATED VERSION CONVERTED BY CHATGPT-------------------------------------
CREATE TABLE user (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    phone VARCHAR(20) NOT NULL UNIQUE,
    email VARCHAR(100) NOT NULL UNIQUE,
    role VARCHAR(20) NOT NULL,
    password VARCHAR(100) NOT NULL
);

CREATE TABLE student (
    id VARCHAR(20) PRIMARY KEY REFERENCES user(id),
    roll VARCHAR(20) NOT NULL UNIQUE,
    dept VARCHAR(20) NOT NULL
);

CREATE TABLE organizer (
    id VARCHAR(20) PRIMARY KEY REFERENCES user(id),
    position VARCHAR(20) NOT NULL,
    responsibility VARCHAR(100) NOT NULL
);

CREATE TABLE accomodation (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(20) NOT NULL UNIQUE,
    location VARCHAR(100) NOT NULL,
    available INTEGER NOT NULL
);

CREATE TABLE mess (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(20) NOT NULL UNIQUE,
    location VARCHAR(100) NOT NULL,
    available INTEGER NOT NULL
);

CREATE TABLE participant (
    id VARCHAR(20) PRIMARY KEY REFERENCES user(id),
    university VARCHAR(100) NOT NULL,
    accomodation_id VARCHAR(20) REFERENCES accomodation(id),
    mess_id VARCHAR(20) REFERENCES mess(id)
);

CREATE TABLE venue (
    name VARCHAR(20) PRIMARY KEY,
    location VARCHAR(100) NOT NULL,
    capacity INTEGER NOT NULL
);

CREATE TABLE event (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(20) NOT NULL UNIQUE,
    type VARCHAR(20) NOT NULL,
    desc VARCHAR(100),
    date TIMESTAMP NOT NULL,
    duration INTERVAL NOT NULL,
    venue VARCHAR(20) REFERENCES venue(name),
    available INTEGER NOT NULL
);

CREATE TABLE registration (
    event_id VARCHAR(20) REFERENCES event(id),
    user_id VARCHAR(20) REFERENCES user(id),
    reg_time TIMESTAMP NOT NULL,
    PRIMARY KEY (event_id, user_id)
);

CREATE TABLE volunteer (
    id VARCHAR(20) REFERENCES student(id),
    event_id VARCHAR(20) REFERENCES event(id),
    PRIMARY KEY (id, event_id)
);

CREATE TABLE manage (
    organizer_id VARCHAR(20) REFERENCES organizer(id),
    event_id VARCHAR(20) REFERENCES event(id),
    PRIMARY KEY (organizer_id, event_id)
);

CREATE TABLE competition (
    id VARCHAR(20) PRIMARY KEY REFERENCES event(id),
    props VARCHAR(100) NOT NULL
);

CREATE TABLE prize (
    competition_id VARCHAR(20) REFERENCES competition(id),
    position INTEGER NOT NULL,
    amount INTEGER NOT NULL,
    winner_id VARCHAR(20) REFERENCES user(id),
    PRIMARY KEY (competition_id, position)
);

CREATE TABLE sponsor (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(20) NOT NULL UNIQUE,
    desc VARCHAR(100)
);

CREATE TABLE sponsorship (
    sponsor_id VARCHAR(20) REFERENCES sponsor(id),
    event_id VARCHAR(20) REFERENCES event(id),
    amount INTEGER NOT NULL,
    PRIMARY KEY (sponsor_id, event_id)
);


-- EVENTS

SELECT * from event;

-- organisers

SELECT user.id, user.name, user.email, organizer.position, organizer.responsibility AS
  O(id, name, email, position, responsibility) FROM user,organizer,manage,event
    WHERE user.id = organizer.id AND organizer_id=manage.organizer_id AND manage.event_id = event_id AND event.id = 'E1';

-- volunteers
SELECT user.id, user.name, user.email AS
  V(id, name, email) FROM user,volunteer,event
    WHERE user.id = volunteer.id AND volunteer.event_id = event.id AND event.id = 'E1';

-- participants
SELECT user.id, user.name, user.email,registration.reg_time AS
  P(id, name, email,reg_time) FROM user,registration,event
    WHERE user.id = registration.user_id AND registration.event_id = event.id AND event.id = 'E1';

-- sponsors
SELECT sponsor.id, sponsor.name, sponsor.desc, sponsorship.amount AS
  S(id, name, desc, amount) FROM sponsor,sponsorship,event
    WHERE sponsor.id = sponsorship.sponsor_id AND sponsorship.event_id = event.id AND event.id = 'E1';

-- Announced Prizes
SELECT prize.position, prize.amount AS
  P(position, amount) FROM prize,competition
    WHERE prize.event_id = event.id AND competition.id = 'E1' 
      ORDER BY prize.position;

-- Winners
SELECT prize.position, user.id, user.name, prize.amount,  AS
  W(position, id, name, amount) FROM user,prize,competition
    WHERE user.id = prize.winner_id AND prize.competition_id = competition.id AND competition.id = 'E1' 
      ORDER BY prize.position;


-- Competition
SELECT competition.id, competition.props AS
  C(id, props) FROM competition,event
    WHERE competition.id = event.id AND event.id = 'E1';



-- EDITS
-- change event_id to competition id in prize
-- in participant change mess to mess_id and accomodation to accomodation_id



-- ACCOMODATION
SELECT * FROM accomodation;

SELECT user.id, user.name, user.email, participant.university AS
  P(id, name, email, university) FROM user,participant,accomodation
    WHERE user.id = participant.id AND participant.accomodation_id = accomodation.id AND accomodation.id = 'A1';
  

-- MESS
SELECT * FROM mess;

SELECT user.id, user.name, user.email, participant.university AS
  P(id, name, email, university) FROM user,participant,mess
    WHERE user.id = participant.id AND participant.mess_id = mess.id AND mess.id = 'M1';






