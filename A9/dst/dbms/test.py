import uuid

from sqlalchemy import String, Integer, ForeignKey
from sqlalchemy.sql import select,insert
from sqlalchemy import text
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.orm import DeclarativeBase, Mapped, mapped_column
from typing_extensions import Annotated
import datetime

# import someting to measure duration


class Base(DeclarativeBase):
    pass


class User(Base):
    __tablename__ = "user"

    id: Mapped[str] = mapped_column(String(20), primary_key=True, unique=True, index=True)
    name: Mapped[str] = mapped_column(String(100), nullable=False)
    phone: Mapped[str] = mapped_column(String(20), nullable=False, unique=True, index=True)
    email: Mapped[str] = mapped_column(
        String(100), nullable=False, unique=True, index=True
    )
    role: Mapped[str] = mapped_column(String(20), nullable=False)
    password: Mapped[str] = mapped_column(String(100), nullable=False)

class Student(Base):
    __tablename__ = "student"
    id: Mapped[str] = mapped_column(ForeignKey("user.id"),primary_key=True)
    roll: Mapped[str] = mapped_column(String(20), nullable=False, unique=True, index=True)
    dept: Mapped[str] = mapped_column(String(20), nullable=False)

class Organizer(Base):
    __tablename__ = "organizer"
    id: Mapped[str] = mapped_column(ForeignKey("user.id"),primary_key=True)
    position: Mapped[str] = mapped_column(String(20), nullable=False)
    responsibility: Mapped[str] = mapped_column(String(100), nullable=False)

class Accomodation(Base):
    __tablename__ = "accomodation"
    id: Mapped[str] = mapped_column(String(20), primary_key=True, unique=True, index=True)
    name: Mapped[str] = mapped_column(String(20), nullable=False, unique=True, index=True)
    location: Mapped[str] = mapped_column(String(100), nullable=False)
    available: Mapped[int] = mapped_column(Integer, nullable=False)

class Mess(Base):
    __tablename__ = "mess"
    id: Mapped[str] = mapped_column(String(20), primary_key=True, unique=True, index=True)
    name: Mapped[str] = mapped_column(String(20), nullable=False, unique=True, index=True)
    location: Mapped[str] = mapped_column(String(100), nullable=False)
    available: Mapped[int] = mapped_column(Integer, nullable=False)    

class Participant(Base):
    __tablename__ = "participant"
    id: Mapped[str] = mapped_column(ForeignKey("user.id"),primary_key=True)
    university: Mapped[str] = mapped_column(String(100), nullable=False)
    accomodation: Mapped[str] = mapped_column(ForeignKey("accomodation.id"))
    mess: Mapped[str] = mapped_column(ForeignKey("mess.id"))    

class Venue(Base):
    __tablename__ = "venue"
    name: Mapped[str] = mapped_column(String(20), primary_key=True, unique=True, nullable=False)
    location: Mapped[str] = mapped_column(String(100), nullable=False)
    capacity: Mapped[int] = mapped_column(Integer, nullable=False)

class Event(Base):
    __tablename__ = "event"
    id: Mapped[str] = mapped_column(String(20), primary_key=True, unique=True, index=True)
    name: Mapped[str] = mapped_column(String(20), nullable=False, unique=True, index=True)
    type: Mapped[str] = mapped_column(String(20), nullable=False)
    desc: Mapped[str] = mapped_column(String(100))
    date: Mapped[datetime.datetime] = mapped_column(nullable=False)
    duration: Mapped[datetime.timedelta] = mapped_column(nullable=False)
    venue: Mapped[str] = mapped_column(ForeignKey("venue.name"))
    available: Mapped[int] = mapped_column(Integer, nullable=False)

class Registration(Base):
    __tablename__ = "registration"
    event_id: Mapped[str] = mapped_column(ForeignKey("event.id"),primary_key=True)
    user_id: Mapped[str] = mapped_column(ForeignKey("user.id"),primary_key=True)
    reg_time: Mapped[datetime.datetime] = mapped_column(nullable=False)

class Volunteer(Base):
    __tablename__ = "volunteer"
    id: Mapped[str] = mapped_column(ForeignKey("student.id"),primary_key=True)
    event_id: Mapped[str] = mapped_column(ForeignKey("event.id"),primary_key=True)
    
class Manage(Base):
    __tablename__ = "manage"
    organizer_id: Mapped[str] = mapped_column(ForeignKey("organizer.id"),primary_key=True)
    event_id: Mapped[str] = mapped_column(ForeignKey("event.id"),primary_key=True)

class Competition(Base):
    __tablename__ = "competition"
    id: Mapped[str] = mapped_column(ForeignKey("event.id"),primary_key=True)
    props: Mapped[str] = mapped_column(String(100), nullable=False)

class Prize(Base):
    __tablename__ = "prize"
    event_id: Mapped[str] = mapped_column(ForeignKey("event.id"),primary_key=True)
    position: Mapped[int] = mapped_column(Integer, primary_key=True, nullable=False)
    amount: Mapped[int] = mapped_column(Integer, nullable=False)
    winner_id: Mapped[str] = mapped_column(ForeignKey("user.id"))

class Sponsor(Base):
    __tablename__ = "sponsor"
    id: Mapped[str] = mapped_column(String(20), primary_key=True, unique=True, index=True)
    name: Mapped[str] = mapped_column(String(20), nullable=False, unique=True, index=True)
    desc: Mapped[str] = mapped_column(String(100))

class Sponsorship(Base):
    __tablename__ = "sponsorship"
    sponsor_id: Mapped[str] = mapped_column(ForeignKey("sponsor.id"),primary_key=True)
    event_id: Mapped[str] = mapped_column(ForeignKey("event.id"),primary_key=True)
    amount: Mapped[int] = mapped_column(Integer, nullable=False)


stmt = select(User).join(Student).where(User.id == Student.id)

venue_insert = "INSERT INTO venue (name, location, capacity) VALUES (:name, :location, :capacity)"
v1 = text(venue_insert).bindparams(name='V1', location='L1', capacity=100).columns(name=str, location=str, capacity=int)



venue_insert = insert(Venue),[
    {'name':'MG_Ground', 'location':'Near Nalanda Complex, Beside SBP Hall', 'capacity':1000},
    {'name': 'TOAT', 'location':'Near MailBuilding', 'capacity':500},
    {'name': 'NetajiAuditorium', 'location':'Near Main Building', 'capacity':1000},
]


# user_insert = insert(User),[
#     {'id':'U001', 'name':'student1', 'phone':'123', 'email':'u1@gmail.com', 'role':'student', 'password':'123'},
#     {'id':'U002', 'name':'student2', 'phone':'124', 'email':'u2@gmail.com', 'role':'student', 'password':'123'},
#     {'id':'U003', 'name':'student3', 'phone':'125', 'email':'u3@gmail.com', 'role':'student', 'password':'123'},
#     {'id':'U004', 'name':'student4', 'phone':'126', 'email':'u4@gmail.com', 'role':'student', 'password':'123'},
#     {'id':'U005', 'name':'student5', 'phone':'127', 'email':'u5@gmail.com', 'role':'student', 'password':'123'},
#     {'id':'U006', 'name':'participant1', 'phone':'128', 'email':'u6@gmail.com', 'role':'participant', 'password':'123'},
#     {'id':'U007', 'name':'participant2', 'phone':'129', 'email':'u7@gmail.com', 'role':'participant', 'password':'123'},
#     {'id':'U008', 'name':'participant3', 'phone':'130', 'email':'u8@gmai.com', 'role':'participant', 'password':'123'},
#     {'id':'U009', 'name':'participant4', 'phone':'131', 'email':'u9@gmail.com', 'role':'participant', 'password':'123'},
#     {'id':'U010', 'name':'participant5', 'phone':'132', 'email':'}
# ]

user_insert = insert(User),[
    {'id':'U001', 'name':'organizer1', 'phone':'1001', 'email':'u1@gmail.com', 'role':'organizer', 'password':'1234'},
    {'id':'U002', 'name':'organizer2', 'phone':'1002', 'email':'u2@gmail.com', 'role':'organizer', 'password':'1234'},
    {'id':'U003', 'name':'student1', 'phone':'1003', 'email':'u3@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U004', 'name':'organizer3', 'phone':'1004', 'email':'u4@gmail.com', 'role':'organizer', 'password':'1234'},
    {'id':'U005', 'name':'participant1', 'phone':'1005', 'email':'u5@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U006', 'name':'organizer4', 'phone':'1006', 'email':'u6@gmail.com', 'role':'organizer', 'password':'1234'},
    {'id':'U007', 'name':'organizer5', 'phone':'1007', 'email':'u7@gmail.com', 'role':'organizer', 'password':'1234'},
    {'id':'U008', 'name':'student2', 'phone':'1008', 'email':'u8@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U009', 'name':'student3', 'phone':'1009', 'email':'u9@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U010', 'name':'participant2', 'phone':'1010', 'email':'u10@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U011', 'name':'participant3', 'phone':'1011', 'email':'u11@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U012', 'name':'participant4', 'phone':'1012', 'email':'u12@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U013', 'name':'student4', 'phone':'1013', 'email':'u13@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U014', 'name':'student5', 'phone':'1014', 'email':'u14@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U015', 'name':'participant5', 'phone':'1015', 'email':'u15@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U016', 'name':'participant6', 'phone':'1016', 'email':'u16@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U017', 'name':'student6', 'phone':'1017', 'email':'u17@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U018', 'name':'student7', 'phone':'1018', 'email':'u18@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U019', 'name':'participant7', 'phone':'1019', 'email':'u19@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U020', 'name':'participant8', 'phone':'1020', 'email':'u20@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U021', 'name':'participant9', 'phone':'1021', 'email':'u21@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U022', 'name':'participant10', 'phone':'1022', 'email':'u22@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U023', 'name':'student8', 'phone':'1023', 'email':'u23@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U024', 'name':'student9', 'phone':'1024', 'email':'u24@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U025', 'name':'student10', 'phone':'1025', 'email':'u25@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U026', 'name':'student11', 'phone':'1026', 'email':'u26@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U027', 'name':'student12', 'phone':'1027', 'email':'u27@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U028', 'name':'student13', 'phone':'1028', 'email':'u28@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U029', 'name':'student14', 'phone':'1029', 'email':'u29@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U030', 'name':'student15', 'phone':'1030', 'email':'u30@gmail.com', 'role':'student', 'password':'1234'},
    {'id':'U031', 'name':'participant11', 'phone':'1031', 'email':'u31@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U032', 'name':'participant12', 'phone':'1032', 'email':'u32@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U033', 'name':'participant13', 'phone':'1033', 'email':'u33@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U034', 'name':'participant14', 'phone':'1034', 'email':'u34@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U035', 'name':'participant15', 'phone':'1035', 'email':'u35@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U036', 'name':'participant16', 'phone':'1036', 'email':'u36@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U037', 'name':'participant17', 'phone':'1037', 'email':'u37@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U038', 'name':'participant18', 'phone':'1038', 'email':'u38@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U039', 'name':'participant19', 'phone':'1039', 'email':'u39@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U040', 'name':'participant20', 'phone':'1040', 'email':'u40@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U041', 'name':'participant21', 'phone':'1041', 'email':'u41@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U042', 'name':'participant22', 'phone':'1042', 'email':'u42@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U043', 'name':'participant23', 'phone':'1043', 'email':'u43@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U044', 'name':'participant24', 'phone':'1044', 'email':'u44@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U045', 'name':'participant25', 'phone':'1045', 'email':'u45@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U046', 'name':'participant26', 'phone':'1046', 'email':'u46@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U047', 'name':'participant27', 'phone':'1047', 'email':'u47@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U048', 'name':'participant28', 'phone':'1048', 'email':'u48@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U049', 'name':'participant29', 'phone':'1049', 'email':'u49@gmail.com', 'role':'participant', 'password':'1234'},
    {'id':'U050', 'name':'participant30', 'phone':'1050', 'email':'u50@gmail.com', 'role':'participant', 'password':'1234'},

    
]

student_insert = insert(Student),[
    {'id':'U003', 'roll':'CSE1', 'dept':'Computer Science and Engineering'},
    {'id':'U008', 'roll':'ECE1', 'dept':'Electrical and Electronics Engineering'},
    {'id':'U009', 'roll':'EEE1', 'dept':'Electrical and Electronics Engineering'},
    {'id':'U013', 'roll':'ME1', 'dept':'Mechanical Engineering'},
    {'id':'U014', 'roll':'CE1', 'dept':'Civil Engineering'},
    {'id':'U017', 'roll':'CSE2', 'dept':'Computer Science and Engineering'},
    {'id':'U018', 'roll':'ECE2', 'dept':'Electronics and Communication Engineering'},
    {'id':'U023', 'roll':'EEE2', 'dept':'Electrical and Electronics Engineering'},
    {'id':'U024', 'roll':'ME2', 'dept':'Mechanical Engineering'},
    {'id':'U025', 'roll':'CE2', 'dept':'Civil Engineering'},
    {'id':'U026', 'roll':'CSE3', 'dept':'Computer Science and Engineering'},
    {'id':'U027', 'roll':'ECE3', 'dept':'Electronics and Communication Engineering'},
    {'id':'U028', 'roll':'EEE3', 'dept':'Electrical and Electronics Engineering'},
    {'id':'U029', 'roll':'ME3', 'dept':'Mechanical Engineering'},
    {'id':'U030', 'roll':'CE3', 'dept':'Civil Engineering'},
]

participant_insert = insert(Participant),[
    {'id':'U005', 'university':'IIT Delhi', 'accomodation_id':'A1', 'mess_id':'M1'},
    {'id':'U010', 'university':'IIT Guwahati', 'accomodation_id':'A1', 'mess_id':'M2'},
    {'id':'U011', 'university':'IIT Bombay', 'accomodation_id':'A2', 'mess_id':'M2'},
    {'id':'U012', 'university':'IIT Bombay', 'accomodation_id':'A3', 'mess_id':'M1'},
    {'id':'U015', 'university':'IIT Delhi', 'accomodation_id':'A2', 'mess_id':'M3'},
    {'id':'U016', 'university':'IIT Guwahati', 'accomodation_id':'A3', 'mess_id':'M2'},
    {'id':'U019', 'university':'IIT Madras', 'accomodation_id':'A2', 'mess_id':'M1'},
    {'id':'U020', 'university':'IIT Kanpur', 'accomodation_id':'A1', 'mess_id':'M2'},
    {'id':'U021', 'university':'IIT Kanpur', 'accomodation_id':'A1', 'mess_id':'M1'},
    {'id':'U022', 'university':'IIT Madras', 'accomodation_id':'A3', 'mess_id':'M3'},
    {'id':'U031', 'university':'IIT Delhi', 'accomodation_id':None, 'mess_id':None},
    {'id':'U032', 'university':'IIT Guwahati', 'accomodation_id':None, 'mess_id':None},
    {'id':'U033', 'university':'IIT Bombay', 'accomodation_id':None, 'mess_id':None},
    {'id':'U034', 'university':'IIT Bombay', 'accomodation_id':None, 'mess_id':None},
    {'id':'U035', 'university':'IIT Delhi', 'accomodation_id':None, 'mess_id':None},
    {'id':'U036', 'university':'IIT Guwahati', 'accomodation_id':None, 'mess_id':None},
    {'id':'U037', 'university':'IIT Madras', 'accomodation_id':None, 'mess_id':None},
    {'id':'U038', 'university':'IIT Kanpur', 'accomodation_id':None, 'mess_id':None},
    {'id':'U039', 'university':'IIT Kanpur', 'accomodation_id':None, 'mess_id':None},
    {'id':'U040', 'university':'IIT Madras', 'accomodation_id':None, 'mess_id':None},
    {'id':'U041', 'university':'IIT Madras', 'accomodation_id':None, 'mess_id':None},
    {'id':'U042', 'university':'IIT Madras', 'accomodation_id':None, 'mess_id':None},
    {'id':'U043', 'university':'IIT Guwahati', 'accomodation_id':None, 'mess_id':None},
    {'id':'U044', 'university':'IIT Madras', 'accomodation_id':None, 'mess_id':None},
    {'id':'U045', 'university':'IIT Madras', 'accomodation_id':None, 'mess_id':None},
    {'id':'U046', 'university':'IIT Kanpur', 'accomodation_id':None, 'mess_id':None},
    {'id':'U047', 'university':'IIT Bombay', 'accomodation_id':None, 'mess_id':None},
    {'id':'U048', 'university':'IIT Madras', 'accomodation_id':None, 'mess_id':None},
    {'id':'U049', 'university':'IIT Delhi', 'accomodation_id':None, 'mess_id':None},
    {'id':'U050', 'university':'IIT Madras', 'accomodation_id':None, 'mess_id':None},
]

registration_insert = insert(Registration),[
    {'event_id':'1', 'user_id':'U005', 'reg_time':datetime.datetime.now()},
    {'event_id':'1', 'user_id':'U010', 'reg_time':datetime.datetime.now()},
    {'event_id':'1', 'user_id':'U011', 'reg_time':datetime.datetime.now()},
    {'event_id':'1', 'user_id':'U012', 'reg_time':datetime.datetime.now()},
    {'event_id':'1', 'user_id':'U015', 'reg_time':datetime.datetime.now()},
    {'event_id':'1', 'user_id':'U016', 'reg_time':datetime.datetime.now()},

    {'event_id':'2', 'user_id':'U012', 'reg_time':datetime.datetime.now()},
    {'event_id':'2', 'user_id':'U015', 'reg_time':datetime.datetime.now()},
    {'event_id':'2', 'user_id':'U016', 'reg_time':datetime.datetime.now()},
    {'event_id':'2', 'user_id':'U019', 'reg_time':datetime.datetime.now()},
    {'event_id':'2', 'user_id':'U020', 'reg_time':datetime.datetime.now()},
    {'event_id':'2', 'user_id':'U021', 'reg_time':datetime.datetime.now()},
    

    # {'event_id':'3', 'user_id':'U019', 'reg_time':datetime.datetime.now()},
    # {'event_id':'3', 'user_id':'U020', 'reg_time':datetime.datetime.now()},
    # {'event_id':'3', 'user_id':'U021', 'reg_time':datetime.datetime.now()},
    # {'event_id':'3', 'user_id':'U022', 'reg_time':datetime.datetime.now()},
    # {'event_id':'3', 'user_id':'U031', 'reg_time':datetime.datetime.now()},
    # {'event_id':'3', 'user_id':'U032', 'reg_time':datetime.datetime.now()},


    {'event_id':'4', 'user_id':'U022', 'reg_time':datetime.datetime.now()},
    {'event_id':'4', 'user_id':'U031', 'reg_time':datetime.datetime.now()},
    {'event_id':'4', 'user_id':'U032', 'reg_time':datetime.datetime.now()},
    {'event_id':'4', 'user_id':'U033', 'reg_time':datetime.datetime.now()},
    {'event_id':'4', 'user_id':'U034', 'reg_time':datetime.datetime.now()},
    {'event_id':'4', 'user_id':'U035', 'reg_time':datetime.datetime.now()},
    

    {'event_id':'5', 'user_id':'U033', 'reg_time':datetime.datetime.now()},
    {'event_id':'5', 'user_id':'U034', 'reg_time':datetime.datetime.now()},
    {'event_id':'5', 'user_id':'U035', 'reg_time':datetime.datetime.now()},
    {'event_id':'5', 'user_id':'U036', 'reg_time':datetime.datetime.now()},
    {'event_id':'5', 'user_id':'U037', 'reg_time':datetime.datetime.now()},
    {'event_id':'5', 'user_id':'U038', 'reg_time':datetime.datetime.now()},

]

prizes_data = [
    {"event_id": "2", "position": 1, "amount": 20000, "winner_id": 'U012'},
    {"event_id": "2", "position": 2, "amount": 15000, "winner_id": 'U016'},
    {"event_id": "2", "position": 3, "amount": 10000, "winner_id": 'U019'},

    {"event_id": "3", "position": 1, "amount": 30000, "winner_id": None},
    {"event_id": "3", "position": 2, "amount": 25000, "winner_id": None},
    {"event_id": "3", "position": 3, "amount": 20000, "winner_id": None},
    {"event_id": "3", "position": 4, "amount": 15000, "winner_id": None},
    {"event_id": "3", "position": 5, "amount": 10000, "winner_id": None},

    {"event_id": "4", "position": 1, "amount": 10000, "winner_id": 'U034'},
    {"event_id": "4", "position": 2, "amount": 7000, "winner_id": 'U033'},
    {"event_id": "4", "position": 3, "amount": 5000, "winner_id": 'U022'}
]



    

'''

from typing import Union

from fastapi import FastAPI, Request
from pydantic import BaseModel

app = FastAPI()


class Item(BaseModel):
    name: str
    price: float
    is_offer: Union[bool, None] = None


@app.get("/")
def read_root(request: Request):
    print(request.body)
    return {"Hello": "World"}


@app.get("/items/{item_id}")
def read_item(item_id: int, q: Union[str, None] = None):
    return {"item_id": item_id, "q": q}


@app.put("/items/{item_id}")
def update_item(item_id: int, item: Item):
    return {"item_name": item.name, "item_id": item_id}

# write sample query in sqlalchemy

e = text("SELECT * from event;")

mgr = text("SELECT user.id,user.name,organizer.position,user.email as M(id,name,position,email) FROM user,organizer,manage,event WHERE (user.id=organizer.id and organizer.id=manage.id and event.id=manage.event_id and event.id=:e_id);")

vtr = text("SELECT user.id,user.name,user.email as V(id,name,email) FROM user,volunteer,event WHERE (user.id=volunteer.id and event.id=volunteer.event_id and event.id=:e_id);")

ppt = text("SELECT user.id,user.name,user.email,participant.reg_time as P(id,name,email,reg_time) FROM user,participant,event WHERE (user.id=participant.id and event.id=participant.event_id and event.id=:e_id);")

spr = text("SELECT sponsor.id,sponsor.name,sponsor.type,sponsor.desc,sponsorship.amount as S(id,name,type,desc,amount) FROM sponsor,sponsorship,event WHERE (sponsor.id=sponsorship.sponsor_id and event.id=sponsorship.event_id and event.id=:e_id);")


'''