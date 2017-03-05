import sqlalchemy as sql
from sqlalchemy.ext.declarative import declarative_base, declared_attr
from sqlalchemy.orm import sessionmaker
from sqlalchemy_utils import database_exists, create_database, drop_database

Base = declarative_base()

class Measurement(Base):
    @declared_attr
    def __tablename__(self):
        return self.__name__
    # Define all Columns of TABLE 'InterferenceSignals'
    id = sql.Column(sql.Integer, primary_key=True)
    timestamp = sql.Column(sql.DateTime)
    color = sql.Column(sql.String)
    salinity_measurement = sql.Column(sql.Integer)

    def __repr__(self):
        return "<Measurement(timestamp='%s', height='%s', error_status='%s'>" % \
               (str(self.timestamp), str(self.height), str(self.status))


def initialize_database(name):
    engine = sql.create_engine('sqlite:///' + name + '.db', echo=True)
    if not database_exists(engine.url):
        create_database(engine.url)
        print('Successfully created ' + name + '.db.')
    else:
        print('\nWARNING: database ' + name + '.db already exists.')
        # input("\nPress ENTER to overwrite, kill the process to keep old database.")
        print('Overwriting...')
        drop_database(engine.url)
        create_database(engine.url)
        print('Successfully created ' + name + '.db.')
    Base.metadata.create_all(engine)
    session = sessionmaker(bind=engine)
    return session(), engine


#Add measurement to measurement table
def add_measurement(session, timestamp, color,salinity_measurement):
    measurement = Measurement(timestamp=timestamp, color=color,salinity_measurement=salinity_measurement)
    session.add(measurement)
