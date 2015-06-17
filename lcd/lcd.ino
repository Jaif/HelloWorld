#include <LiquidCrystal.h>
#include <MySensor.h>
#include <Time.h> 
#include <SPI.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 8);
MySensor gw;

boolean timeReceived = false;
unsigned long lastUpdate=0, lastRequest=0, lastTempRequest=0;
float temp1, temp2;
boolean showingTemp = false;

void setup() {
  lcd.begin(16, 2);
  lcd.print("Temp!");

  gw.begin(incomingMessage, 99);
  gw.sendSketchInfo("LCD", "1.0");
  gw.requestTime(receiveTime);
}

void loop() {
  unsigned long now = millis();
  gw.process();

   // If no time has been received yet, request it every 10 second from controller
  // When time has been received, request update every hour
  if ((!timeReceived && now-lastRequest > 10*1000)
    || (timeReceived && now-lastRequest > 60*1000*60)) {
    // Request time from controller. 
    Serial.println("requesting time");
    gw.requestTime(receiveTime);  
    lastRequest = now;
  }
  
  if((second() / 10) % 2 == 0){
    printTime(now);
  }else{
    printTemp();
  }
}

void receiveTime(unsigned long time) {
  setTime(time);
  timeReceived = true;
}

void incomingMessage(const MyMessage &message) {
  if (message.type == V_TEMP){
      if(message.sensor == 0){
        temp1=message.getFloat();
      }
      else if(message.sensor == 1){
        temp2=message.getFloat();
      }
      
      showingTemp = false;
  }
  
  // We only expect one type of message from controller. But we better check anyway.
  /*
  if (message.type==V_LIGHT) {
     // Change relay state
     digitalWrite(message.sensor, message.getBool()?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     gw.saveState(message.sensor, message.getBool());
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   }
  */ 
}

void printTemp() {
  if(!showingTemp){
    showingTemp = true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp1: ");
    lcd.print(temp1);
    lcd.write(223);
    lcd.print("C");
    lcd.setCursor(0,1);
    lcd.print("Temp2: ");
    lcd.print(temp2); 
    lcd.write(223);
    lcd.print("C");
  }
}

void printTime(unsigned long now) {
  showingTemp = false;
  // Print time every second
  if (timeReceived && now-lastUpdate > 1000) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(year()); 
    printLcdDigits(month(), '-');
    printLcdDigits(day(), '-');
    lcd.setCursor(0,1);
    printLcdDigits(hour());
    printLcdDigits(minute(),':');
    printLcdDigits(second(),':');
    
    lastUpdate = now;
  }
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void printLcdDigits(int digits, char separator){
  // utility function for digital clock display: prints preceding colon and leading 0
  lcd.print(separator);
  printLcdDigits(digits);
}

void printLcdDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);
}
