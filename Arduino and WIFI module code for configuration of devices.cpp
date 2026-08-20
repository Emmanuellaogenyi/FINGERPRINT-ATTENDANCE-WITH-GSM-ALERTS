#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#include <HTTPClient.h>

#include <Preferences.h>
Preferences pref;

// #include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include <jsonlib.h>

// #include <Wire.h>
// #include <ErriezDS1307.h>

// Create DS1307 RTC object
// ErriezDS1307 rtc;

// // Global date/time object
// struct tm dt;

#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

uint8_t id;

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>

#define OLED_RESET -1

Adafruit_SSD1305 display(132, 64, &Wire, OLED_RESET);

#define rxPin 2
#define txPin 4
#define BAUD_RATE 9600
HardwareSerial sim800(1);

const char* PARAM_MESSAGE = "data";
String postMessage = "";
String Signal = "";
const char* ma;

AsyncWebServer server(80);

String matric = "";
String phone = "";
int no;
int check = 0;

int print = 0;

const char* postR = "http://192.168.4.1/postR";

String result = "";

String name = ""; 
// uint8_t readnumber(void) {
// uint8_t num;
// // no = matric.toInt();
// num = 8;

// return num;
// }
int sms = 0;


void notFound(AsyncWebServerRequest *request) 
{
  request->send(404, "text/plain", "The page not found");
}

String readD() {

  return result;
}

void setup() {
 Serial.begin(115200);
 WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
 Serial.println();

 if ( ! display.begin(0x3C) ) {
     Serial.println("Unable to initialize OLED");
     while (1) yield();
  }
  sim800.begin(BAUD_RATE, SERIAL_8N1, rxPin, txPin);
  Serial.println("SIM800L serial initialize");
  // init done
  // display.display(); // show splashscreen
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(40, 10);
  display.println("Smart");

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,32);
  display.println("Attendance");

  display.display();

 finger.begin(57600);
 
 if (finger.verifyPassword()) {
   Serial.println("Found fingerprint sensor!");
  } else {
  Serial.println("Did not find fingerprint sensor :(");
   while (1) { delay(1); }
  }
  
  pref.begin("data", false);

  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP("Smart Attendance");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    if(request->hasParam(PARAM_MESSAGE, true))
    {
      postMessage = request->getParam(PARAM_MESSAGE, true)->value();
      
    }
    request->send_P(200, "text/plain", readD().c_str());
  });

  server.onNotFound(notFound);

  server.begin();

  check = 1;
}

void loop() {

  Serial.print("postMessage: ");
  Serial.println(postMessage);

  Signal = jsonExtract(postMessage, "signal");
  Serial.print("signal: ");
  Serial.println(Signal);

  if (Signal == "A" ) {
    print = 1;
  }

   if (print == 1){
   display.clearDisplay();   // clears the screen and buffer
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor(40, 10);
   display.println("Class");

   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor(10,32);
   display.println("Attendance");

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(25,50);
   display.println("Place your thumb");

   display.display();

  //  String CourseT = jsonExtract(postMessage, "courseT");
  //  Serial.print("CourseT: ");
  //  Serial.println(CourseT);

  //  String CourseC = jsonExtract(postMessage, "courseC");
  //  Serial.print("CourseC: ");
  //  Serial.println(CourseC);

   getFingerprintID();
   
  }

  if (Signal == "B") {
    print = 2;
   display.clearDisplay();   // clears the screen and buffer
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor(35, 10);
   display.println("Arrival");

   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor(10,32);
   display.println("Attendance");

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(25,50);
   display.println("Place your thumb");

   display.display();

   getFingerprintIDA();
  }

  if (Signal == "C") {
    print = 2;
   display.clearDisplay();   // clears the screen and buffer
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(40, 10);
   display.println("Thumbprint");

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(40,24);
   display.println("Enrollment");

   display.display();

   delay(3000);

   name = jsonExtract(postMessage, "name");
   Serial.print("name: ");
   Serial.println(name);

   matric = jsonExtract(postMessage, "matric");
   Serial.print("matric: ");
   Serial.println(matric);

   phone = jsonExtract(postMessage, "phone");
   Serial.print("phone: ");
   Serial.println(phone);

   no = matric.toInt();
   
   id = no;

   getFingerprintEnroll();
   
  }

  if (Signal == "E") {
    print = 2;
    display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(40, 10);
  display.println("Smart");

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,32);
  display.println("Attendance");

  display.display();
  }

  if (Signal == "Z") {
    result = "";
  }

  if (sms == 1) {
    SendMessage();
    sms = 2;
  }

  if (Signal == "F" ) {
    pref.clear();
  }
}

uint8_t getFingerprintEnroll() {
 int p = -1;
   display.clearDisplay();   // clears the screen and buffer
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(40, 10);
   display.println("Wating for");

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(40,24);
   display.println("Valid Finger");

   display.display();

 Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
 while (p != FINGERPRINT_OK) {
 p = finger.getImage();
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image taken");
 break;
 case FINGERPRINT_NOFINGER:
 Serial.println(".");
 break;
 case FINGERPRINT_PACKETRECIEVEERR:
 Serial.println("Communication error");
 break;
 case FINGERPRINT_IMAGEFAIL:
 Serial.println("Imaging error");
 break;
 default:
 Serial.println("Unknown error");
 break;
 }
 }
 // OK success!
 p = finger.image2Tz(1);
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image converted");
 break;
 case FINGERPRINT_IMAGEMESS:
 Serial.println("Image too messy");
 return p;
 case FINGERPRINT_PACKETRECIEVEERR:
 Serial.println("Communication error");
 return p;
 case FINGERPRINT_FEATUREFAIL:
 Serial.println("Could not find fingerprint features");
 return p;
 case FINGERPRINT_INVALIDIMAGE:
 Serial.println("Could not find fingerprint features");
 return p;
 default:
 Serial.println("Unknown error");
 return p;
 }
   display.clearDisplay();   // clears the screen and buffer

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(40,24);
   display.println("Remove finger");

   display.display();
 Serial.println("Remove finger");
 delay(2000);
 p = 0;
 while (p != FINGERPRINT_NOFINGER) {
 p = finger.getImage();
 }
 Serial.print("ID "); Serial.println(id);
 p = -1;
   display.clearDisplay();   // clears the screen and buffer
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(20, 10);
   display.println("Place same finger");

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(50,24);
   display.println("again");

   display.display();
 Serial.println("Place same finger again");
 while (p != FINGERPRINT_OK) {
 p = finger.getImage();
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image taken");
 break;
 case FINGERPRINT_NOFINGER:
 Serial.print(".");
 break;
 case FINGERPRINT_PACKETRECIEVEERR:
 Serial.println("Communication error");
 break;
 case FINGERPRINT_IMAGEFAIL:
 Serial.println("Imaging error");
 break;
 default:
 Serial.println("Unknown error");
 break;
 }
 }
 // OK success!
 p = finger.image2Tz(2);
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image converted");
 break;
 case FINGERPRINT_IMAGEMESS:
 Serial.println("Image too messy");
 return p;
 case FINGERPRINT_PACKETRECIEVEERR:
 Serial.println("Communication error");
 return p;
 case FINGERPRINT_FEATUREFAIL:
 Serial.println("Could not find fingerprint features");
 return p;
 case FINGERPRINT_INVALIDIMAGE:
 Serial.println("Could not find fingerprint features");
 return p;
 default:
 Serial.println("Unknown error");
 return p;
 }
 // OK converted!
   display.clearDisplay();   // clears the screen and buffer
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10, 10);
   display.println("Creating model for #");

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(30,24);
   display.println("Matric No");

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(100,24);
   display.println(id);

   display.display();
 Serial.print("Creating model for #"); Serial.println(id);
 p = finger.createModel();
 if (p == FINGERPRINT_OK) {
 Serial.println("Prints matched!");
 } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
 Serial.println("Communication error");
 return p;
 } else if (p == FINGERPRINT_ENROLLMISMATCH) {
 Serial.println("Fingerprints did not match");
 return p;
 } else {
 Serial.println("Unknown error");
 return p;
 }
 Serial.print("ID "); Serial.println(id);
 p = finger.storeModel(id);
 if (p == FINGERPRINT_OK) {
   display.clearDisplay();   // clears the screen and buffer 
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(30,24);
   display.println("Model Stored");

   display.display();
 Serial.println("Stored!");
 } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
 Serial.println("Communication error");
 return p;
 } else if (p == FINGERPRINT_BADLOCATION) {
 Serial.println("Could not store in that location");
 return p;
 } else if (p == FINGERPRINT_FLASHERR) {
 Serial.println("Error writing to flash");
 return p;
 } else {
 Serial.println("Unknown error");
 return p;
 }

  ma = matric.c_str();
  
  pref.putString(ma, postMessage);
  String result = pref.getString(ma, "None");
  Serial.println(result);

  delay(3000);

  postMessage =  "{\"signal\":\"E\",\"name\":\" \",\"matric\":\"0\",\"phone\":\" \"}";

  return true;
 }


 uint8_t getFingerprintID() {
 uint8_t p = finger.getImage();
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image taken");
 break;
 case FINGERPRINT_NOFINGER:
 Serial.println("No finger detected");
 
 return p;
  // case FINGERPRINT_PACKETRECIEVEERR:
  // Serial.println("Communication error");
  // return p;
 case FINGERPRINT_IMAGEFAIL:
 Serial.println("Imaging error");
 return p;
  // default:
  // Serial.println("Unknown error");
  // return p;
 }
 // OK success!
 p = finger.image2Tz();
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image converted");
 break;
 case FINGERPRINT_IMAGEMESS:
 Serial.println("Image too messy");
 return p;
  
 }
 // OK converted!
 p = finger.fingerSearch();
 if (p == FINGERPRINT_OK) {
 Serial.println("Found a print match!");
 } 

 else if (p == FINGERPRINT_NOTFOUND) {
 Serial.println("Did not find a match");
 return p;
 }

 // found a match!
 Serial.print("Found ID #"); Serial.print(finger.fingerID);
 Serial.print(" with confidence of "); Serial.println(finger.confidence);

  String Ma = (String)finger.fingerID;
    ma = Ma.c_str();

//  if (Signal == "A") {
    String res = pref.getString(ma, "None");;

    result = "{\"file\":"+res+"}";
    Serial.println(result);
    
   name = jsonExtract(result, "name");
   Serial.print("name: ");
   Serial.println(name);

   matric = jsonExtract(result, "matric");
   Serial.print("matric: ");
   Serial.println(matric);

   phone = jsonExtract(result, "phone");
   Serial.print("phone: ");
   Serial.println(phone);

   sms = 1;
   
   display.clearDisplay();   // clears the screen and buffer
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10, 10);
   display.println(name);

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(10,24);
   display.println("Successfully marked");

   display.display(); 
   delay(1500);
 return p;  
}

uint8_t getFingerprintIDA() {
 uint8_t p = finger.getImage();
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image taken");
 break;
 case FINGERPRINT_NOFINGER:
 Serial.println("No finger detected");
 
 return p;
  // case FINGERPRINT_PACKETRECIEVEERR:
  // Serial.println("Communication error");
  // return p;
 case FINGERPRINT_IMAGEFAIL:
 Serial.println("Imaging error");
 return p;
  // default:
  // Serial.println("Unknown error");
  // return p;
 }
 // OK success!
 p = finger.image2Tz();
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image converted");
 break;
 case FINGERPRINT_IMAGEMESS:
 Serial.println("Image too messy");
 return p;
  
 }
 // OK converted!
 p = finger.fingerSearch();
 if (p == FINGERPRINT_OK) {
 Serial.println("Found a print match!");
 } 

 else if (p == FINGERPRINT_NOTFOUND) {
 Serial.println("Did not find a match");
 return p;
 }

 // found a match!
 Serial.print("Found ID #"); Serial.print(finger.fingerID);
 Serial.print(" with confidence of "); Serial.println(finger.confidence);

  String Ma = (String)finger.fingerID;
    ma = Ma.c_str();

//  if (Signal == "A") {
    String res = pref.getString(ma, "None");;

    // result = "{\"file\":"+res+"}";
    // Serial.println(result);
    
   name = jsonExtract(res, "name");
   Serial.print("name: ");
   Serial.println(name);

   matric = jsonExtract(res, "matric");
   Serial.print("matric: ");
   Serial.println(matric);

   phone = jsonExtract(res, "phone");
   Serial.print("phone: ");
   Serial.println(phone);

   SendMessageA();
   
   display.clearDisplay();   // clears the screen and buffer
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(20, 10);
   display.println(name);

   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(30,24);
   display.println("Successfully ");
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(35,40);
   display.println("Checking-in");

   display.display(); 
   delay(1500);
   return p;
  }

void SendMessage()
{
  Serial.println("Sending message");
  sim800.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  sim800.println("AT+CMGS=\""+phone+"\"\r"); // Replace x with mobile number
  delay(1000);
  sim800.println("Your child " +name+ "just checked-in for class");// The SMS text you want to send
  delay(100);
   sim800.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void SendMessageA()
{
  Serial.println("Sending message");
  sim800.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  sim800.println("AT+CMGS=\""+phone+"\"\r"); // Replace x with mobile number
  delay(1000);
  sim800.println("Your child " +name+ "just checked-in for arrival");// The SMS text you want to send
  delay(100);
   sim800.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
