from AggiE_Database import *
import datetime
import numpy as np

session,engine = initialize_database('test')
n = 20
start_time = datetime.datetime.now()
current_time = start_time
colors = ['red','green','blue']
red_salinity = np.linspace(250,750,n)
for i in range(n):

    # Take a separate measurement in each tank
    noise = 20*np.random.random_sample()-10
    add_measurement(session,current_time,colors[0],red_salinity[i]+noise)
    noise = 20 * np.random.random_sample() - 10
    add_measurement(session,current_time,colors[1],500+noise)
    noise = 20 * np.random.random_sample() - 10
    add_measurement(session,current_time,colors[2],red_salinity[n-1-i]+noise)
    current_time += datetime.timedelta(seconds=.5)

session.commit()