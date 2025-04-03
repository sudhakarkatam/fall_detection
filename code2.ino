#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <LiquidCrystal.h>
#include <TinyGPS.h>
TinyGPS gps;
int x, y, z;
int buzzer = 10;
String msg;
float flat;
float flon;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
int force=11;
void setup() {
  Serial.begin(9600);

  pinMode( buzzer,OUTPUT);
   pinMode( force,INPUT);
 digitalWrite(buzzer, LOW);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("emergency alert");
  lcd.setCursor(0, 1);
  lcd.print(" epilepsy");
  delay(2000);
  if (!accel.begin()) {
    lcd.print("ADXL345 not found");
    //Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    while (1)
      ;
  }
}
void loop() {
  sensors_event_t event;
  accel.getEvent(&event);
  x = event.acceleration.x;
  y = event.acceleration.y;
  z = event.acceleration.z;


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("X=");
  lcd.print(x);
  lcd.print("Y=");
  lcd.print(y);
  lcd.setCursor(0, 1);
  lcd.print("Z=");
  lcd.print(z);
  delay(1000);
  int force_value=digitalRead(force);
   lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("force_value: ");
  lcd.print(force_value);
  delay(1000);


  if (event.acceleration.x > 7 || event.acceleration.x < -7) {
    lcd.clear();
    lcd.print("UNSTABLE BODY");
    lcd.setCursor(0, 1);
    lcd.print("DETECTED");

    digitalWrite(buzzer, HIGH);
    delay(4000);
    digitalWrite(buzzer, LOW);
    msg = "UNSTABLE POSTURE";
    SendMessage();
  } else if (event.acceleration.y > 7 || event.acceleration.y < -7) {
    lcd.clear();
    lcd.print("UNSTABLE BODY");
    lcd.setCursor(0, 1);
    lcd.print("DETECTED");

    digitalWrite(buzzer, HIGH);
    delay(4000);
    digitalWrite(buzzer, LOW);
    msg = "UNSTABLE POSTURE";
    SendMessage();

    delay(5000);
  }
  else if(force_value==0)
  {
     lcd.clear();
    lcd.print("fall");
    lcd.setCursor(0, 1);
    lcd.print("detected!");
    digitalWrite(buzzer, HIGH);
    delay(2000);
    digitalWrite(buzzer, LOW);
    msg = "fall detected!";
    SendMessage();

    delay(5000);
    
  }
   else {
    digitalWrite(buzzer, LOW);
    //    digitalWrite(Relay, LOW);
  }


  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;) {
    while (Serial.available()) {
      char c = Serial.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c))  // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData) {
    //    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.println("LAT=");
    Serial.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    //    Serial.println(flat);
    Serial.println(" LON=");
    Serial.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    //    Serial.println(flon);
    delay(2000);
  }

 
}


void SendMessage() {
  Serial.println("Sending Message...");

  Serial.println("AT");
  delay(1000);
  Serial.println("ATE0");
  delay(1000);
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+919959190405\"\r");
  delay(2500);

  String flatStr = String(flat, 6);
  String flonStr = String(flon, 6);

  Serial.println(msg);
  Serial.println("Location: Click to view");
  Serial.println("http://www.google.com/maps/place/" + flatStr + "," + flonStr);
  delay(6000);
  Serial.write(26);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Message sent..");
  Serial.println("Message sent..");

  delay(5000);  // Wait for call duration
  Serial.println("ATD7013451264;");
  Serial.println("Emergency Call Triggered!");
  delay(30000);           // Wait for call duration
  Serial.println("ATH");  // Hang up
  delay(1000);
}
