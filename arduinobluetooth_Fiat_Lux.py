import serial
ser1 = serial.Serial('/dev/rfcomm0', 9600)
my_object = ""
while True:
    my_bytes = ser1.readline()
    incoming_int = str(my_bytes, "utf-8")
    #print ("Sensoror:" + " Flow:" + " Flow_Rate:" + " " + incoming_int + " Units:"+" L/Hr")
    print (incoming_int)

    print (ser1.readline())
    "{\"Sensor\":\"flow\",\"flow_rate\":%d,\"units\":\"L/Hr\"}\n",Calc
