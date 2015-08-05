# Urban-Kitchen-Garden
This is the repository for the Arduino Urban Kitchen Garden Project

V1
 Adding RTC support
 RTC Connected to VCC and Ground
 I2C Connections:
 RTC SDA connected to pin Analog 4
 RTC SCL connected to pin Analog 5

Used I2CScanner.ino to find I2C addresses- 0x50 (EEPROM?)  and 0x68 found
Used SetRTC.ino to manually set proper date/date on RTC module- This wil set the date/time as soon as serial 
is initiated so set ahead by a minute and wait till time matches then open serial window to set.
Downloaded from http://projectsfromtech.blogspot.com/

V2 Added DHT 11 Support 
Example testing sketch for various DHT humidity/temperature sensors
Written by ladyada, public domain
Connect pin 1 (on the left) of the sensor to +5V
Connect pin 2 of the sensor to whatever your DHTPIN is
Connect pin 4 (on the right) of the sensor to GROUND
Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

V3 Test of digital output on pin 13 for light function WORKS

V4 Added Temp Control Pin 12 with relay control, setpoint and hysteresis

V5 Made all variables global and changed format
Moved Serial prints to their own area

V6 Added Moisture Measurement Pin3
Altered hysteresis/setpoint logic for relays

V7 Change Pin Assigments for relays 
Added Nokia 5110 Display
Pin 11 for contrast on the Nokia 5110

V8 Added String Texts for statuses

V9 Oct 21 2014
Note- For Serial data you must send a space+ hard return to initialize the connection- Unknown why

V10 Oct 21 2014
Added Overtemp alarm- flashes backlight

V11 Oct 21 2014
Fixed Overtemp string/variables
Added Undertemp OR Overtemp condition to alarm which flashes LCD backlight
ActiveAlarm string printed to serial and can be used later to flag alarm state

V12 Oct 21 2014
backlight variables added- Will make it psossible for user input via encoders etc

V13
Alarms and Display broken out into their own voids
Alarm code can be used for any failure and all above code 0 will flash LCD backlight
Corrected time display so when value < 10 it will add a 0 in front

V14 Changed cooling setpoints to be cooler
Moved lightpin to D10
Inverted Soil moisture for sensor

V15 June 2015 
More cleanup and minor edits
Removed H Bridge for heating option (not necessary in my home)
Removed function for humidity addition (ultrasonic fog fountain element)- unecessary in my build (humidity remains in sealed chamber, sometimes to excess)
Removed peristaltic pump drive- unecessary at this time but should be added back in later
changed Arduino back to Nano & altered needed I/O
Commented the heck out of everything so others with far more logical brains can make sense of my insanity