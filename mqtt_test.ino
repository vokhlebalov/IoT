#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

/*
  TODO
  1. Добавить переподключение клиента при ошибке подключения
  2. Выделить генерацию данных в отдельную функцию
  3. Добавить дополнительные переменные для отправки
  4. 
  
*/

// Function prototypes
//void subscribeReceive(char* topic, byte* payload, unsigned int length);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

// Set your MAC address and IP address here
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 160);
 
// Make sure to leave out the http and slashes!
const char* server = "82.148.31.128";

//mqtt.eclipseprojects.io
//www.mqtt-dashboard.com
//test.mosquitto.org
// 82.148.31.128

// Ethernet and MQTT related objects
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

const char clientID[] = "someClient123";
const char outTopic[] = "mega_1/temp";
const char inTopic[] = "mega_1/temp";

bool setMQTTConnection() {
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  } else {
    Serial.print("  DHCP assigned IP "); Serial.println(Ethernet.localIP());
  }

  // give the Ethernet shield a second to initialize:
  delay(3000);
  Serial.print("connecting to "); Serial.print(server); Serial.println("...");
  
  // Set the MQTT server to the server stated above ^
  mqttClient.setServer(server, 1883);   
  // Establish the subscribe event
  mqttClient.setCallback(callback);
  
  // Attempt to connect to the server with the clientID
  if (mqttClient.connect(clientID)) 
  {
    Serial.println("Connection has been established, well done");
  } 
  else 
  {
    Serial.println("Looks like the server connection failed...");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//void subscribeReceive(char* topic, byte* payload, unsigned int length)
//{
//  // Print the topic
//  Serial.print("Topic: ");
//  Serial.println(topic);
// 
//  // Print the message
//  Serial.print("Message: ");
//  for(int i = 0; i < length; i ++)
//  {
//    Serial.print(char(payload[i]));
//  }
// 
//  // Print a newline
//  Serial.println("");
//}
