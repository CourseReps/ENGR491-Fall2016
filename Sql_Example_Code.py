import sqlite3

conn = sqlite3.connect('example.db')

curse = conn.cursor()

#Create table
curse.execute('''CREATE TABLE stocks(date text, trans text, symbol text, qty real, price real)''')

#Insert a row of data 
curse.execute("INSERT INTO stocks VALUES ('2006-01-05','BUY','RHAT',100,35.14)")

#Save (commit) the changes
conn.commit()

conn.close()
