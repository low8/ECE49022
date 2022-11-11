import machine
import network
import esp32
import socket
import espnow

# Global variables
temp = esp32.raw_temperature()
hall = esp32.hall_sensor()
led = machine.Pin(13, machine.Pin.OUT)
led.value(1)
if led.value() == 1:
    red_led_state = "ON"
else:
    red_led_state = "OFF" # string, check state of red led, ON or OFF


ssid = "Tim's iPhone"
password = "password3211"
timer4Counter = 0
timer5Counter = 0

station = network.WLAN(network.STA_IF)
station.active(True)

e = espnow.ESPNow()
e.active(True)
peer = b'\xC4\xDD\x57\xCA\x4E\xE8'
e.add_peer(peer)

if(not station.isconnected()):
    print("Connecting")
    station.connect(ssid, password)
    while(not station.isconnected()):
        pass
    
print("Connected to", ssid)
print("IP address:", station.ifconfig()[0])

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(5)

def web_page():
    """Function to build the HTML webpage which should be displayed
    in client (web browser on PC or phone) when the client sends a request
    the ESP32 server.
    
    The server should send necessary header information to the client
    (YOU HAVE TO FIND OUT WHAT HEADER YOUR SERVER NEEDS TO SEND)
    and then only send the HTML webpage to the client.
    
    Global variables:
    temp, hall, red_led_state
    """
    temp = esp32.raw_temperature()
    hall = esp32.hall_sensor()

    
    html_webpage = """<!DOCTYPE HTML><html>
    <head>
    <title>ESP32 Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h1 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.5rem; }
    .sensor-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
    .button {
        display: inline-block; background-color: #e7bd3b; border: none; 
        border-radius: 4px; color: white; padding: 16px 40px; text-decoration: none;
        font-size: 30px; margin: 2px; cursor: pointer;
    }
    .button2 {
        background-color: #4286f4;
    }
    </style>
    </head>
    <body>
    <h1>Team 34 Cornhole Dashboard</h1>
    <p>
    <i class="fas fa-bolt" style="color:#E4001B;"></i> 
    <span class="sensor-labels">Red Score</span> 
    <span>"""+str(temp)+"""</span>
    <sup class="units">Points</sup>
    </p>
    <p>
    <i class="fas fa-bolt" style="color:#0099FF;"></i>
    <span class="sensor-labels">Blue Score</span>
    <span>"""+str(hall)+"""</span>
    <sup class="units">Points</sup>
    </p>
    <p>
    RED LED Current State: <strong>""" + red_led_state + """</strong>
    </p>
    <p>
    <a href="/?red_led=on"><button class="button">RED ON</button></a>
    </p>
    <p>
    <a href="/?red_led=off"><button class="button">RED OFF</button></a>
    </p>
    </body>
    </html>"""
    return html_webpage

while True:
    host, msg = e.recv()
    if msg:
        print(host, msg)
        conn, addr = s.accept()
        request = conn.recv(1024)
        conn.settimeout(None)
        request = str(request)
        print(request)
        if request[16] == "o" and request[17] == "n":
            red_led_state = "On"
            led.value(1)
        elif request[16] == "o" and request[17] == "f" and request[18] == "f":
            red_led_state = "Off" # string, check state of red led, ON or OFF
            led.value(0)
        response = web_page()
        conn.send('HTTP/1.1 200 OK\n')
        conn.send('Content-Type: text/html\n')
        conn.send('Connection: close\n\n')
        conn.sendall(response)
        if msg == b'end':
            break
        
