// ************************************************************************* 
// **  a module from openhab-trains to work 2 semaphore signals           **
// **                                                          **
// **                                                                     **
// **  https://github.com/thegavs/openhab-trains                          **
// **                                                                     **
// **  Change "signal_3 & 4" to something that makes sense to you.        **
// **                                                                     **
// **  Change SSID, PASSWORD to your wifi network.                        **
// **                                                                     **
// **  Change IPADDRESS to the ip address of the MQTT server.             **
// **  if only using one network the SSID2 and 3 can be left as is.       **
// **                                                                     **
// **  Look for //**changeme**//                                          **
// **                                                                     **
// **  The signal start and end position and LED brightness can be        **
// **  changed in openhab rules                                           **
// ** v1.0 initial realease.                                              **
// ************************************************************************* 

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>


// DEFINE HERE THE KNOWN NETWORKS
const char* KNOWN_SSID[] = {"SSID1", "SSID2", "SSID3"}; //**changeme**//
const char* KNOWN_PASSWORD[] = {"PASSWORD1", "PASSWORD2", "PASSWORD3"}; //**changeme**//
const char* KNOWN_MQTT[] = {"IPADDRESS1", "IPADDRESS2", "IPADDRESS3"}; //**changeme**//
 
const int   KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); // number of known networks

char* topic_1 = "signal_3"; //**changeme**//
char* topic_2 = "signal_4"; //**changeme**//
 
WiFiClient espClient;
PubSubClient client(espClient);
 
String message;
char* receivedChar;
int volts_in;
float volts_out;
float volt_mod = 95.83;
char message_buff[10];
int pos = 0;    // variable to store the servo position
const int analogOutPin = 0; // Analog output pin that the LED is attached to D3 nodemcu

int on_off;
int low;
int high;
int bounce;
int led_brighness;

int led1 = 14;   // arduino pin 14 nodemcu pin D5
int led2 = 12;  // arduino pin 12 modemcu pin D6

String temp;

Servo homearm;
Servo shuntarm;


void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");

 for (int i=0;i<length;i++) 
 {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    message = message + String(receivedChar);
  }
  //String led_char = String(receivedChar);

 // Serial.println();
  //Serial.print(topic);
 // Serial.print(message);
 // Serial.println();

  temp = message.substring(0,1);
  on_off = temp.toInt();
  Serial.println(on_off);

  temp = message.substring(2,5);
  low = temp.toInt();
  Serial.println(low);

  temp = message.substring(6,9);
  high = temp.toInt();
  Serial.println(high);

  temp = message.substring(10,13);
  led_brighness = temp.toInt();
  Serial.println(led_brighness);

  bounce = high - 15;
  
  if (strcmp(topic,topic_1) == 0) 
    {
      //Serial.print("inside direction");
      Serial.println(message);
      if (on_off == 0)
      {
        Serial.print("inside 0");
        Serial.println();
        for (pos = low; pos <= high; pos += 1)   
        { // goes from 0 degrees to 180 degrees
                                  // in steps of 1 degree
          homearm.write(pos);              // tell servo to go to position in variable 'pos'
          delay(3);  
        }
        for (pos = high; pos >= bounce; pos -= 1) 
        { // goes from 180 degrees to 0 degrees
          homearm.write(pos);              // tell servo to go to position in variable 'pos'
          delay(5);                       // waits 15ms for the servo to reach the position
        }
        for (pos = bounce; pos <= high; pos += 1) 
        { // goes from 180 degrees to 0 degrees
          homearm.write(pos);              // tell servo to go to position in variable 'pos'
          delay(3);                       // waits 15ms for the servo to reach the position
        }
      }
      else
      {
        for (pos = high; pos >= low; pos -= 1) 
        { // goes from 180 degrees to 0 degrees
          homearm.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
      }
      analogWrite(led1,led_brighness );
    }
  if (strcmp(topic, topic_2) == 0) 
   {
    analogWrite(led2,led_brighness );
    if (on_off == 0) // off or up or danger
      {
        Serial.print("inside 0");
        Serial.println();
        for (pos = low; pos <= high; pos += 1)   
        { // goes from 0 degrees to 180 degrees
                                  // in steps of 1 degree
          shuntarm.write(pos);              // tell servo to go to position in variable 'pos'
          delay(3);  
        }
        for (pos = high; pos >= bounce; pos -= 1) 
        { // goes from 180 degrees to 0 degrees
          shuntarm.write(pos);              // tell servo to go to position in variable 'pos'
          delay(5);                       // waits 15ms for the servo to reach the position
        }
        for (pos = bounce; pos <= high; pos += 1) 
        { // goes from 180 degrees to 0 degrees
          shuntarm.write(pos);              // tell servo to go to position in variable 'pos'
          delay(3);                       // waits 15ms for the servo to reach the position
        }
      }
      else // on or down or go or green
      {
        for (pos = high; pos >= low; pos -= 1) 
        { // goes from 180 degrees to 0 degrees
          shuntarm.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
      }
      

   }
   message = "";
 //  move();
}
 
 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266 Client")) {
  Serial.println("connected");
  // ... and subscribe to topic
  client.subscribe(topic_1);
  client.subscribe(topic_2);
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}



 
void setup()
{
  boolean wifiFound = false;
  int i, n;

  Serial.begin(115200);

  // ----------------------------------------------------------------
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  // ----------------------------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");

  // ----------------------------------------------------------------
  // WiFi.scanNetworks will return the number of networks found
  // ----------------------------------------------------------------
  Serial.println(F("scan start"));
  int nbVisibleNetworks = WiFi.scanNetworks();
  Serial.println(F("scan done"));
  if (nbVisibleNetworks == 0) {
    Serial.println(F("no networks found. Reset to try again"));
    while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // ----------------------------------------------------------------
  // if you arrive here at least some networks are visible
  // ----------------------------------------------------------------
  Serial.print(nbVisibleNetworks);
  Serial.println(" network(s) found");

  // ----------------------------------------------------------------
  // check if we recognize one by comparing the visible networks
  // one by one with our list of known networks
  // ----------------------------------------------------------------
  for (i = 0; i < nbVisibleNetworks; ++i) {
    Serial.println(WiFi.SSID(i)); // Print current SSID
    for (n = 0; n < KNOWN_SSID_COUNT; n++) { // walk through the list of known SSID and check for a match
      if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str())) {
        Serial.print(F("\tNot matching "));
        Serial.println(KNOWN_SSID[n]);
      } else { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound) break; // break from the "for each visible network" loop
  } // end for each visible network

  if (!wifiFound) {
    Serial.println(F("no Known network identified. Reset to try again"));
    while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // ----------------------------------------------------------------
  // if you arrive here you found 1 known SSID
  // ----------------------------------------------------------------
  Serial.print(F("\nConnecting to "));
  Serial.println(KNOWN_SSID[n]);

  // ----------------------------------------------------------------
  // We try to connect to the WiFi network we found
  // ----------------------------------------------------------------
  WiFi.begin(KNOWN_SSID[n], KNOWN_PASSWORD[n]);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  // ----------------------------------------------------------------
  // SUCCESS, you are connected to the known WiFi network
  // ----------------------------------------------------------------
  Serial.println(F("WiFi connected, your IP address is "));
  Serial.println(WiFi.localIP());
  Serial.println(KNOWN_MQTT[n]);
 
  client.setServer(KNOWN_MQTT[n], 1883);    // MQTT setup
  client.setCallback(callback);

  homearm.attach(4); //home arm on D4 or D2 nodemcu
  shuntarm.attach(5);   // shunt arm on D5 or D1 nodemcu
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  analogWrite(led1,256 );
  analogWrite(led2,256 );
}
 
void loop()
{
 if (!client.connected()) {
  reconnect();
 }
 client.loop();
}
