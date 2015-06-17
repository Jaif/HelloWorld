// Simple binary switch example 
// Connect button or door/window reed switch between 
// digitial I/O pin 3 (BUTTON_PIN below) and GND.

#include <MySensor.h>
#include <SPI.h>
#include <Bounce2.h>

#define CHILD_ID 3
#define BUTTON_PIN  3  // Arduino Digital I/O pin for button/reed switch

#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay
#define LED_PIN  7
#define LED_ID  7
#define TEMP_LED_PIN 6
#define TEMP_LED_ID 6

MySensor gw;
Bounce debouncer = Bounce(); 
int oldValue=-1;

// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(CHILD_ID,V_TRIPPED);

void setup()  
{  
  gw.begin(incomingMessage, 10);

 // Setup the button
  pinMode(BUTTON_PIN,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN,HIGH);
  
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN, gw.loadState(LED_ID)?RELAY_ON:RELAY_OFF);
  
  pinMode(TEMP_LED_PIN, OUTPUT);
  digitalWrite(TEMP_LED_PIN, LOW);
  
  // After setting up the button, setup debouncer
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);
  
  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  gw.present(CHILD_ID, S_DOOR);  
  gw.present(LED_ID, S_LIGHT);
  gw.present(TEMP_LED_ID, S_LIGHT);
}


//  Check if digital input has changed and send in new value
void loop() 
{
  gw.process();
  
  debouncer.update();
  // Get the update value
  int value = debouncer.read();
 
  if (value != oldValue) {
     // Send in the new value
     gw.send(msg.set(value==HIGH ? 1 : 0));
     oldValue = value;
  }
} 

void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
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
}

