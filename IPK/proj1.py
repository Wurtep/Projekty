import sys
import socket
import json

# check arguments
if (len(sys.argv) != 3):
	print("This script requires valid api key and existing city given as api_key=<API KEY> and city=<city name>, please try again.")
	sys.exit()

# assign args to variables that are correctly named
api_key = sys.argv[1];
city = sys.argv[2]

# set HOST and PORT
HOST = 'api.openweathermap.org'
PORT = 80

# make request from api using city and api_key provided from user
request = "GET /data/2.5/weather?q=" + city + "&APPID=" + api_key + "&units=metric HTTP/1.1\r\nHost: api.openweathermap.org\r\n\r\n"
byte_request = str.encode(request)

# try to create socket, exit on failure
try:
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
except:
	print("Socket creation failed")
	sys.exit()

# try to connect to server, close socket and exit on failure
try:
	s.connect((HOST, PORT))
except:
	print("Connection to server failed")
	s.close()
	sys.exit()

# try to send requst to server, close socket and exit on failure
try:
	s.sendall(byte_request)
except:
	print("An error occured while sending request to the server")
	s.close()
	sys.exit()

# try to receive response, close socket and exit on failure
try:
	data = str(s.recv(1024))
except:
	print("An error occured while receiving data from the server")
	s.close()
	sys.exit()

# just closing the socket
s.close()

# find beginning of json format
firstBracket = data.find('{')
# load data in json format
datajson = json.loads(data[firstBracket:-1])

# code 200 means that data were succesfully retrieved
if "200" in str(datajson['cod']):
	# if user will ask for something like pRague the api will still find data for Prague
	# by using .title() making sure it will print correctly, e.g. new york will be printed as New York
	print(city.title())

# every other code number means something went wrong, about what the user will be notified
else:
	print("Error:", datajson['cod'])
	print("Message:", datajson['message'])
	sys.exit()

# printing the data if they are available
# if not, the user should be able to tell
if "description" in datajson['weather'][0]:
	print(datajson['weather'][0]['description'])
else:
	print("No data")

if "temp" in datajson['main']:
	print("temp: ", datajson['main']['temp'], " degree Celsius", sep='')
else:
	print("No temperature")

if "humidity" in datajson['main']:
	print("humidity: ", datajson['main']['humidity'],"%", sep='')
else:
	print("No humidity")

if "pressure" in datajson['main']:
	print("pressure: ", datajson['main']['pressure'], "hPa", sep='')
else:
	print("No pressure")

if "speed" in datajson['wind']:
	print("wind-speed: ", datajson['wind']['speed']*3.6, "km/h", sep='')
else:
	print("wind-speed: 0km/h")

if "deg" in datajson['wind']:
	print("wind-deg: ", datajson['wind']['deg'], sep='')
else:
	print("wind-deg: n/a")