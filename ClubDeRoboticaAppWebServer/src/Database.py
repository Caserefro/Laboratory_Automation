from sqlalchemy import *
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import declarative_base
from sqlalchemy.orm import sessionmaker
from datetime import datetime
import pytz

Base = declarative_base()
engine = create_engine('sqlite:///test.db')  # Use an in-memory SQLite database for testing
Base.metadata.create_all(engine)  # Create all tables
Session = sessionmaker(bind=engine)
mexico_tz = pytz.timezone("America/Mexico_City")


class User(Base):
    __tablename__ = 'users'

    user_id = Column(Integer, primary_key=True)
    username = Column(String(26), nullable=False, default="")
    password = Column(String(128), nullable=False, default="")
    name = Column(String(128), nullable=False, default="")
    ncid = Column(Integer, nullable=False, default="")
    email = Column(String(128), nullable=False, default="")
    phone_number = Column(Integer, nullable=False, default="")

    def __init__(self, user_id, username, password, name, ncid, email,phone_number):
        self.user_id = user_id
        self.username = username
        self.password = password
        self.name = name
        self.ncid = ncid
        self.email = email
        self.phone_number = phone_number


class E_Device(Base):
    __tablename__ = 'e_devices'

    device_id = Column(Integer, primary_key=True)
    ip_address = Column(String(26), nullable=False, default="")
    device_type = Column(String(61), nullable=False, default="")
    place = Column(String(61), nullable=False, default="")

    def __init__(self, device_id, ip_address, device_type, tag, place, description,):
        self.device_id = device_id
        self.ip_address = ip_address
        self.device_type = device_type
        self.place = place


class Item(Base):
    __tablename__ = 'items'

    item_id = Column(Integer, primary_key=True, autoincrement=True)
    item_name = Column(String(26), nullable=False, default="")
    description = Column(String(255), nullable=False, default="")
    quantity = Column(Integer, nullable=False, default=0)
    type = Column(String(61), nullable=False, default="")

    def __init__(self, item_name, description, quantity, type):
        self.item_name = item_name
        self.description = description
        self.quantity = quantity
        self.type = type


class AnnouncedItem(Base):
    __tablename__ = 'announcements'

    a_id = Column(Integer, primary_key=True, autoincrement=True)
    title = Column(String(255), nullable=False, default="")
    body = Column(Text, nullable=False, default="")
    date = Column(DateTime, nullable=False, default=lambda: datetime.now(mexico_tz))
    color = Column(String(61), nullable=False, default="")
    icon = Column(String(61), nullable=False, default="")
    facebook_link = Column(String(255), nullable=True)
    gforms_link = Column(String(255), nullable=True)

    def __init__(self, title, body, color, icon, facebook_link=None, gforms_link=None):
        self.title = title
        self.body = body
        self.color = color
        self.icon = icon
        self.facebook_link = facebook_link
        self.gforms_link = gforms_link

class AccessRecord(Base):
    __tablename__ = 'access_records'

    record_id = Column(Integer, primary_key=True, autoincrement=True)
    accessed_e_device_id = Column(Integer, nullable=False)
    requesting_device_id = Column(String(11), nullable=False)  # U## if user ID E## if Edevice
    access_time = Column(DateTime, nullable=False, default=lambda: datetime.now(mexico_tz))
    door_status = Column(Integer, nullable=False)  # 0 = closed, 1 = open

    def __init__(self, accessed_e_device_id, door_status):
        self.accessed_e_device_id = accessed_e_device_id
        self.door_status = door_status


class BorrowedItemsRecord(Base):
    __tablename__ = 'borrowed_items'

    record_id = Column(Integer, primary_key=True, autoincrement=True)
    item_id = Column(Integer, nullable=False)
    user_id = Column(Integer, nullable=False)
    borrow_date = Column(DateTime, nullable=False, default=lambda: datetime.now(mexico_tz))
    return_date = Column(DateTime, nullable=True)

    def __init__(self, item_id, user_id, return_date=None):
        self.item_id = item_id
        self.user_id = user_id
        self.return_date = return_date

class BaseRecord(Base):
    __abstract__ = True

    ar_id = Column(Integer, primary_key=True, autoincrement=True)
    sensor_id = Column(Integer, ForeignKey('devices.device_id'), nullable=True)
    time = Column(DateTime, nullable=False, default=lambda: datetime.now(mexico_tz))
    tag = Column(String(51), nullable=False, default="")


class WeatherData(Base):
    __tablename__ = 'weather_records'

    record_id = Column(Integer, primary_key=True, autoincrement=True)
    windspeed = Column(Float, nullable=False)
    wind_direction = Column(Integer, nullable=False)
    uv_index = Column(Float, nullable=False)
    temperature = Column(Float, nullable=False)
    rain_gauge = Column(Float, nullable=False)
    rel_hum = Column(Float, nullable=False)
    time = Column(DateTime, nullable=False, default=lambda: datetime.now(mexico_tz))

    def __init__(self, windspeed, wind_direction, uv_index, temperature, rain_gauge, rel_hum):
        self.windspeed = windspeed
        self.wind_direction = wind_direction
        self.uv_index = uv_index
        self.temperature = temperature
        self.rain_gauge = rain_gauge
        self.rel_hum = rel_hum


class ElectricRecord(Base):
    __tablename__ = 'electric_records'

    record_id = Column(Integer, primary_key=True, autoincrement=True)
    current = Column(Float, nullable=False)
    time = Column(DateTime, nullable=False, default=lambda: datetime.now(mexico_tz))

    def __init__(self, current):
        self.current = current
