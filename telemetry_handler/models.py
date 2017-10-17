from sqlalchemy import *
from sqlalchemy.orm import (scoped_session, sessionmaker, relationship, backref)

from sqlalchemy.ext.declarative import declarative_base

engine = create_engine('sqlite:///telem.db', convert_unicode=True)
db_session = scoped_session(sessionmaker(autocommit=False, autoflush=False, bind=engine))

Base = declarative_base()

Base.query = db_session.query_property()

class Telemetry(Base):
    __tablename__ = 'Telemetry'
    timestamp = Column(BigInteger, primary_key=True)
    subsystem = Column(String)
    param = Column(String)
    value = Column(BigInteger)
