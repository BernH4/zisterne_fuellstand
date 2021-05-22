#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "credentials.h" //My Wifi and MQTT credentials
 
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;
const char* mqttServer = "192.168.178.143";
const int mqttPort = 1883;
const char* mqttUser = MQTT_USERN;
const char* mqttPassword = MQTT_PASSWD;


#define trigPin 4
#define echoPin 5
#define relayPin 12

long duration;
int distance;
uint32_t measureTime, WiFiTime;
char payload[10];

// Static IP details...Use static because it's much faster
IPAddress ip(192, 168, 178, 200);
IPAddress gateway(192, 168, 178, 1);
IPAddress mask(255, 255, 255, 0);

 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  /* WiFi.mode(WIFI_OFF); */
  /* WiFi.forceSleepBegin(); */
  /* delay( 1 ); */

  Serial.begin(9600); //serialdebug

  measureTime = millis(); //serialdebug
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Power on devices needed for measurement
  
  /* //JSN SR04T */
  Serial.println("Starting measurrment"); //serialdebug
  /* // Clear the trigPin by setting it LOW: */
  digitalWrite(trigPin, LOW);
  
  delayMicroseconds(5);

 /* // Trigger the sensor by setting the trigPin high for 10 microseconds: */
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  digitalWrite(relayPin, LOW);
  
  // Calculate the distance:
  distance = duration*0.034/2;
  measureTime = millis() - measureTime; //serialdebug
  /* delay(10); //DELETE ME */
  Serial.print("Time needed for measurement : "); //serialdebug
  Serial.println(measureTime); //serialdebug
  Serial.println("Finished measurrement"); //serialdebug
 
 
  WiFiTime=millis();
  /* WiFi.forceSleepWake(); */
  /* delay(1); */
  // Disable the WiFi persistence. 
  // The ESP8266 will not load and save WiFi settings in the flash memory.
  /* WiFi.persistent( false ); */
  WiFi.mode(WIFI_STA);
  WiFi.config(ip,gateway,mask);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
  /* while (WiFi.waitForConnectResult() != WL_CONNECTED) { */
    delay(5);
    /* Serial.print("Connecting to WiFi..Code: "); //serialdebug */
    Serial.print(WiFi.status()); //serialdebug
    if ((millis() - WiFiTime) > 5000) {
      Serial.println("Timout wifi connection, going to sleep.."); //serialdebug
      ESP.deepSleepInstant(10*1000000,WAKE_NO_RFCAL);
    }
  }
  WiFiTime = millis() - WiFiTime; //serialdebug
  Serial.print("Time needed for wifi connection: "); //serialdebug
  Serial.println(WiFiTime); //serialdebug
  Serial.println("Connected to the WiFi network"); //serialdebug
 
  client.setServer(mqttServer, mqttPort);
  /* client.setCallback(callback); */
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT..."); //serialdebug
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("connected");   //serialdebug
    }
    else {
      if (millis() > 10000) {
        Serial.println("Timout MQTT connection, going to sleep.."); //serialdebug
        ESP.deepSleepInstant(10*1000000,WAKE_NO_RFCAL);
      }
      Serial.print("failed with state "); //serialdebug
      Serial.print(client.state()); //serialdebug
      delay(5);
    }
  }
 

  Serial.println("now publishing..."); //serialdebug
  /* client.publish("zisterne/fuelstand", String(distance).c_str()); */
  sprintf(payload, "%d,%d", distance, millis()); //csv distance and runtime
  client.publish("zisterne/fuelstand", payload);
  /* client.publish("zisterne/fuelstand", "test"); */
  delay(10);
  Serial.print("Full cycle time: "); //serialdebug
  Serial.println(millis()); //serialdebug
  Serial.println("finished, going to sleep.."); //serialdebug
  /* ESP.deepSleep(10e6, WAKE_NO_RFCAL); */
  /* ESP.deepSleep(0); */
  ESP.deepSleepInstant(10*1000000,WAKE_NO_RFCAL);
  /* WiFi.disconnect( true ); */
  /* delay( 1 ); */
/* // WAKE_RF_DISABLED to keep the WiFi radio disabled when we wake up */
  /* ESP.deepSleepInstant(30*1000000,WAKE_RF_DISABLED); */
  delay(100);
}

 
void loop(){}
