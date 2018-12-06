// ************************************************************************* 
// **  a module from openhab-trains to read the position of a lever       **
// **  or switch.                                                         **
// **                                                                     **
// **  https://github.com/thegavs/openhab-trains                          **
// **                                                                     **
// **  Change "lever_name" to something that makes sense to you.          **
// **                                                                     **
// **  Change SSID, PASSWORD to your wifi network.                        **
// **                                                                     **
// **  Change IPADDRESS to the ip address of the MQTT server.             **
// **  if only using one network the SSID2 and 3 can be left as is.       **
// **                                                                     **
// **  Look for //**changeme**//                                          **
// ************************************************************************* 



#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


 
// DEFINE HERE THE KNOWN NETWORKS add your netork here
const char* KNOWN_SSID[] = {"SSID1", "SSID2", "SSID3"}; //**changeme**//
const char* KNOWN_PASSWORD[] = {"PASSWORD1", "PASSWORD2", "PASSWORD3"}; //**changeme**//
const char* KNOWN_MQTT[] = {"IPADDRESS1", "IPADDRESS2", "IPADDRESS3"}; //**changeme**//

const int   KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); // number of known networks

char* topic_1 = "lever_name/1"; //**changeme**//
char* topic_2 = "lever_name/2"; //**changeme**//
char* topic_3 = "lever_name/3"; //**changeme**//
char* topic_4 = "lever_name/4"; //**changeme**//
char* topic_5 = "lever_name/5"; //**changeme**//
char* topic_6 = "lever_name/6"; //**changeme**//
char* topic_7 = "lever_name/7"; //**changeme**//
char* topic_8 = "lever_name/8"; //**changeme**//

String clientName = "lever_name"; //**changeme**//


int lever1 = 5;  // D1 nodemcu
int lever2 = 4;  // D2 nodemcu
int lever3 = 0;  // D3 nodemcu
int lever4 = 2;  // D4 nodemcu
int lever5 = 14;  // D5 nodemcu
int lever6 = 12;  // D6 nodemcu
int lever7 = 13;  // D7 nodemcu
int lever8 = 3;  // TX nodemcu

int lever1_val = 0;
int lever2_val = 0;
int lever3_val = 0;
int lever4_val = 0;
int lever5_val = 0;
int lever6_val = 0;
int lever7_val = 0;
int lever8_val = 0;

int lever1_old_val = 1;
int lever2_old_val = 1;
int lever3_old_val = 1;
int lever4_old_val = 1;
int lever5_old_val = 1;
int lever6_old_val = 1;
int lever7_old_val = 1;
int lever8_old_val = 1;



WiFiClient espClient;
PubSubClient client(espClient);
 

char message_buff[10];
int pos = 0;    // variable to store the servo position



 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("signal box")) 
 {
  Serial.println("connected");
  // ... and subscribe to topic
  // client.subscribe("demo_signal_home");
  // client.subscribe("demo_signal_shunt");
 } 
 else 
 {
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


 // pinMode(volt_switch, OUTPUT);           // Volt switch if used with resitor bridge
  pinMode(lever1, INPUT_PULLUP);
  pinMode(lever2, INPUT_PULLUP);
  pinMode(lever3, INPUT_PULLUP);
  pinMode(lever4, INPUT_PULLUP);
  pinMode(lever5, INPUT_PULLUP);
  pinMode(lever6, INPUT_PULLUP);
  pinMode(lever7, INPUT_PULLUP);
  pinMode(lever8, INPUT_PULLUP);
}
 
void loop()
{
 if (!client.connected()) 
 {
    reconnect();
 }
 client.loop();

  delay(1000);
 
 lever1_val = digitalRead(lever1);
 if (lever1_old_val != lever1_val)
 {
    lever1_old_val = lever1_val;
    if (lever1_val == 1)
    {
      Serial.println("off");
      client.publish(topic_1, "0"); 
    }
    else
    {
      Serial.println("on");
      client.publish(topic_1, "1"); 
    }
 }

 lever2_val = digitalRead(lever2);
 if (lever2_old_val != lever2_val)
 {
    lever2_old_val = lever2_val;
    if (lever2_val == 1)
    {
      client.publish(topic_2, "0"); 
    }
    else
    {
      client.publish(topic_2, "1"); 
    }
 }

 lever3_val = digitalRead(lever3);
 if (lever3_old_val != lever3_val)
 {
    lever3_old_val = lever3_val;
    if (lever3_val == 1)
    {
      client.publish(topic_3, "0"); 
    }
    else
    {
      client.publish(topic_3, "1"); 
    }
 }
 
  lever4_val = digitalRead(lever4);
 if (lever4_old_val != lever4_val)
 {
    lever4_old_val = lever4_val;
    if (lever4_val == 1)
    {
      client.publish(topic_4, "0"); 
    }
    else
    {
      client.publish(topic_4, "1"); 
    }
 }
 
 lever5_val = digitalRead(lever5);
 if (lever5_old_val != lever5_val)
 {
    lever5_old_val = lever5_val;
    if (lever5_val == 1)
    {
      client.publish(topic_5, "0"); 
    }
    else
    {
      client.publish(topic_5, "1"); 
    }
 }

 lever6_val = digitalRead(lever6);
 if (lever6_old_val != lever6_val)
 {
    lever6_old_val = lever6_val;
    if (lever6_val == 1)
    {
      client.publish(topic_6, "0"); 
    }
    else
    {
      client.publish(topic_6, "1"); 
    }
 }

 
 lever7_val = digitalRead(lever7);
 if (lever7_old_val != lever7_val)
 {
    lever7_old_val = lever7_val;
    if (lever7_val == 1)
    {
      client.publish(topic_7, "0"); 
    }
    else
    {
      client.publish(topic_7, "1"); 
    }
 }

 lever8_val = digitalRead(lever8);
 if (lever8_old_val != lever8_val)
 {
    lever8_old_val = lever8_val;
    if (lever8_val == 1)
    {
      client.publish(topic_8, "0"); 
    }
    else
    {
      client.publish(topic_8, "1"); 
    }
 }
}
