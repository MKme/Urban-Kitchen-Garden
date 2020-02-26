/* 
Erics Open Source Urban Kitchen Garden
My Youtube Channel  : http://www.youtube.com/mkmeorg
If you use this code or personalize it etc- please consider sharing it back with the world via the forum at http://www.mkme.org/forum

Please consider purchasing some of the needed components for this project from Adafruit.com- They are an amazing company who support projects like this one
every day.  They also provided the needed libraries below free of charge for all us to use.  Consider supporting them whenever you can.   http://adafruit.com 

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

V15 
June 2015 More cleanup and minor edits
Removed H Bridge for heating option (not necessary in my home)
Removed function for humidity addition (ultrasonic fog fountain element)- unecessary in my build (humidity remains in sealed chamber, sometimes to excess)
Removed peristaltic pump drive- unecessary at this time but should be added back in later
changed Arduino back to Nano & altered needed I/O
Commented the heck out of everything so others with far more logical brains can make sense of my insanity

*/

#include "Wire.h" //RTC
#define DS1307_ADDRESS 0x68 //For RTC Support
byte zero = 0x00; //workaround for issue #527 on RTC
#include "DHT.h"  //You need this library
#define DHTPIN 2     // DHT11 Humidity Pin
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);//unknown needed by DHT11
#include "Adafruit_GFX.h"  //Awesome Adafruit libraries- download from their site
#include "Adafruit_PCD8544.h"  //Awesome Adafruit libraries- download from their site
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);//4 and 3 are reversed on the cheap eBay models

//LCD display & backlight control 
#define backlightpin 11 //Pin for LCD backlight
int backlightlevel = 220; //Level for backlight PWM
int backlightoff = 255; //Level which turns backlight off completely
int displaycontrast = 50; //***** Set this contrast to suit your display- they are indeed different

//LED Light Control
#define LIGHTPIN 10 //Pin for light relay
int lighton =4; //Lights On Hour Value
int lightoff =21; //Lights Off Hour Value (Goes one hour past for some reason Eric was too dumb to figure out)

//Temperature Control
#define TempPin 12 // Relay pin for cooling
int TempHyst=1; //Hysteresis for cooling system in Degrees
int setTemp=24; //Temp Setpoint in Degrees C (29=84.2 so a 10 deg f drop at night = 23.4C at night)
                // 75 deg is supposed to be top of range for herbs (23.9C) so 10 deg drop would be 65 (18.33C)
                //This is just handy to know stuff if you want killer growth

//Globalize variables
int second, minute, hour, weekDay, monthDay, month, year;
int h;//humidity used to be float for decimal places
int t;//temperature used to be float for decimal places

//Alarms
int alarmseconds; // Used for overtemp alarm seconds since active for display flashing
int AlarmTemp = 4; // Alarm temp OVER or UNDER setTemp value
int ActiveAlarm; // Used for overtemp alarms to notify etc

/*
Alarm Codes:
0 = No active alarm
1 = Internal Temp <> AlarmTemp from setTemp value (over or under temp condition)
2 = TBD (Previously High/Low Humidity)
3 = TBD (Previously High/Low soil moisture)
4 = TBD (Previously too long heating/cooling cycle)
5 = TBD (Previously too long/short watering interval)
6 = TBD (Previously Float switch on water tank/water level low)
7 = TBD (Previously Light state error)
8 = TBD (Previously Watchdog/Reboot Triggered)
*/


//Moisture Measurement
#define moisturePin 3 //pin for analog moisture sensor
int moisture;  //Set the integer for moisture

//String Texts for use in statuses throughout code
String LightStatus;
String CoolingStatus;
String SoilStatus;


void setup(){  //Do setup type stuff to make cooler stuff work later--------------------------
  Wire.begin();
  Serial.begin(9600);
  dht.begin();//initialize temp and humidity
  pinMode(TempPin, OUTPUT);
  pinMode(LIGHTPIN, OUTPUT);
  analogWrite(backlightpin,backlightlevel);// PWM of LCD backlight but ebay unit is backwards-   //must go high + cycle to dim   //Very Dim=230
  display.begin();//Display code
  display.setContrast(displaycontrast);//Nokia 5110 works best at 50- no more flicker
  delay(1000);  //party foul I know but it is only setup
  display.clearDisplay();     // clears the screen and buffer- makes gooder stuff show up later
  display.setTextSize(1);     // set text size
  display.setTextColor(BLACK);
  
 // Splash Screen - Insert random funny here
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("   Eric's");
  display.println("");
  display.println("   Arduino");
  display.println("   Garden");
  display.println("   Thing");
  display.display();
  delay(2000);
  display.clearDisplay();     // clears the screen and buffer
  
 }

void loop(){
  TimeFunctions();   //Do the time related stuff
  TempFunctions();   //Do the temp related stuff
  debug();           //Figure out what has broken type of stuffs
  SoilMoisture();    //Check the soil moisture
  LCDDisplay();      //Send everything to the LCD display
  Alarm();           //Activate alarm if applicable
  DisplayFlash();    //Flash display if alarm active
  
}


//------------------------------This needed by RTC
byte decToBcd(byte val){  //Needed for RTC
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {  //Needed for RTC
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}
//------------------------------End needed by RTC


//Do time related stuffs so cool stuffs work gooder------------------------------
void TimeFunctions(){ 
  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  second = bcdToDec(Wire.read());
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  monthDay = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());

//Lights ON and OFF Functions---------------------
   if(hour >= lighton && hour <= lightoff)
   {digitalWrite(LIGHTPIN, LOW);  // set the Light Relay on
   LightStatus = "On";
   }
   else 
   {digitalWrite(LIGHTPIN, HIGH); 
   LightStatus = "Off"; } }

//Do temperature related stuffs----------------------------------------------------------
void TempFunctions(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  t = dht.readTemperature();
    
//Cooling circuit code here----------------------------------------------------------------
//this needs a safety timer/shutoff function in production version

   if (t >= (setTemp + TempHyst)) {
    digitalWrite(TempPin, LOW);
    CoolingStatus = "On";
    }
   if (t <= (setTemp - TempHyst)) {
   digitalWrite(TempPin, HIGH);
   CoolingStatus = "Off";
   } }

//take soil moisture measurement---------------------------
void SoilMoisture(){ 
   moisture = analogRead(moisturePin);
   if (moisture >= 700) SoilStatus = ("Dry");
   if (moisture < 400) SoilStatus = ("Wet");
   if (moisture >= 400 && moisture <700 ) SoilStatus =  ("Moist");}

//Do debugging stuffs for nerds only-----------------------------------------------------------------
void debug(){  //This area only used for debugging any changes to the code- handy dandy sometimes :)
  if (Serial.available()){ //Can leave debug active as no resources used when not connected
  //Date and Time Values   3/1/11 23:59:59
  Serial.print(month);
  Serial.print("/");
  Serial.print(monthDay);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  if (hour < 10) Serial.print("0");
  Serial.print(hour);
  Serial.print(":");
  if (minute < 10) Serial.print("0");
  Serial.print(minute);
  Serial.print(":");
  if (second < 10) Serial.print("0");
  Serial.println(second);
    //humidity Sensor Values
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT");} 
    else {
      Serial.print("Humidity: "); 
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(t);
      Serial.println(" *C"); }
//Cooling System Values
  Serial.print ("Cooling is ");
  Serial.println (CoolingStatus);
//Light System Values
  Serial.print("Lights "); 
  Serial.println (LightStatus);
//Soil Moisture Values
  Serial.print("Soil Moisture: "); 
  Serial.print(SoilStatus);
  Serial.println (moisture);
//Alarm Status
  Serial.print("Active Alarm "); 
  Serial.println(ActiveAlarm);
}}

//Write everything to the LCD so folks can see what's up---------------------------------
void LCDDisplay(){
  display.clearDisplay();              // clears the screen and buffer
  display.setCursor(0,0);
  display.setTextSize(1);     // set text size
  display.print(month);
  display.print("/");
  display.print(monthDay);
  //display.print("/");
  //display.print(year);
  display.print(" ");
  if (hour < 10) display.print("0");
  display.print(hour);
  display.print(":");
  if (minute < 10) display.print("0");
  display.println(minute);
  //display.print(":");
  //if (second < 10) display.print("0");
  //display.println(second);
  display.println("");
  display.print(t);
  display.print("*C  ");
  display.print(h);
  display.println("% RH");
  display.print("Light: ");
  display.println(LightStatus);
  display.print("Cooling: ");
  display.println(CoolingStatus);
  display.print("Soil: ");
  display.print(SoilStatus);
  display.print(" ");
  display.print(moisture);
  display.display();
  }
   
void Alarm(){
  //Overtemp OR Undertemp alarm--------------------------------------
  if (t >= setTemp + AlarmTemp  || t <= setTemp - AlarmTemp && second != alarmseconds  ) {ActiveAlarm = 1;} //Checks over or under alarm limit and setas alarm #1
  else   ActiveAlarm = 0; //No active alarm criteria met
  
  //Insert Soil Alarm here
  
  //Insert Moisture Alarm here
  
  //Insert non desired output alarms here
  
  //Insert SkyNet has taken over the world via my garden alarm here

}

//Flash the display if any alarm is active------------------------------
void DisplayFlash(){
  if (ActiveAlarm >0  && second != alarmseconds){
  alarmseconds = second; 
  analogWrite(backlightpin,backlightoff);// Turn off backlight so flashing can begin
  }
  //Else Normal backlight- no more flashing
  else 
  analogWrite(backlightpin,backlightlevel);// Normal PWM of LCD backlight
  }
