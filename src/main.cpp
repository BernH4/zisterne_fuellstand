// TODO: multiple measuerements and get average, try again when > 950
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "credentials.h" //My Wifi and MQTT credentials
#include "myheaders.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;
const char* mqttServer = "192.168.178.143";
const int mqttPort = 1883;
const char* mqttUser = MQTT_USERN;
const char* mqttPassword = MQTT_PASSWD;


#define trigPin 4
#define echoPin 5
#define relayPin 12
#define voltagePin A0
/* #define SLEEPTIME 30*60*1000000 */
#define SLEEPTIME 30*60*1000000
/* #define TIMEOUT 20000 */
#define TIMEOUT 200000 //TEMPORARY


long duration;
int distance;
int bat_raw;
float analog_batt;
uint32_t measureTime, WiFiTime;
char payload[21];

// Static IP details...Use static because it's much faster
IPAddress ip(192, 168, 178, 200);
IPAddress gateway(192, 168, 178, 1);
IPAddress mask(255, 255, 255, 0);


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  //Serial.begin(115200); //serialdebug

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); //turn off by default

  /* if (ESP.getResetReason().equals("External System")) { */
  /*   digitalWrite(LED_BUILTIN, LOW); */
  /*   Serial.println("Reset Reason = Button, will search for updates after measurement"); */
  /* } */

  //measureTime = millis(); //serialdebug
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Power on devices needed for measurement

  //JSN SR04T
  //Serial.println("Starting measurrment"); //serialdebug
  // Clear the trigPin by setting it LOW:
  digitalWrite(relayPin, HIGH);
  for ( int i = 0;;i++)  {
    //Serial.print("Try: "); //serialdebug
    //Serial.println(i); //serialdebug
    digitalWrite(trigPin, LOW);

    delayMicroseconds(5);

    // Trigger the sensor by setting the trigPin high for 10 microseconds:
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
    duration = pulseIn(echoPin, HIGH);

    //Measure battery voltage
    bat_raw = analogRead(voltagePin);
    //Serial.print("Battery RAW: "); //serialdebug
    //Serial.println(bat_raw); //serialdebug
    analog_batt = (3.749/893) * bat_raw;
    //Serial.print("Battery Voltage: "); //serialdebug
    //Serial.println(analog_batt); //serialdebug

    // Calculate the distance:
    distance = duration*0.034/2;

    //Serial.println(duration); //serialdebug
    //Serial.print("Distance: "); //serialdebug
    //Serial.println(distance); //serialdebug
    delay(10);
    if ((distance > 5 && distance < 900) || i > 5) {
      break;
    }
  }
  digitalWrite(relayPin, LOW);
  //measureTime = millis() - measureTime; //serialdebug
  /* delay(10); //DELETE ME */
  //Serial.print("Time needed for measurement : "); //serialdebug
  //Serial.println(measureTime); //serialdebug
  //Serial.println("Finished measurrement"); //serialdebug


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
    ///* Serial.print("Connecting to WiFi..Code: "); //serialdebug */
    //Serial.print(WiFi.status()); //serialdebug
    /* if ((millis() - WiFiTime) > 5000) { */
    if ((millis() - WiFiTime) > TIMEOUT) {
      //Serial.println("Timout wifi connection, going to sleep.."); //serialdebug
    ESP.deepSleepInstant(SLEEPTIME, WAKE_NO_RFCAL);
    }
  }
  //WiFiTime = millis() - WiFiTime; //serialdebug
  //Serial.print("Time needed for wifi connection: "); //serialdebug
  //Serial.println(WiFiTime); //serialdebug
  //Serial.println("Connected to the WiFi network"); //serialdebug

  client.setServer(mqttServer, mqttPort);
  /* client.setCallback(callback); */

  while (!client.connected()) {
    //Serial.println("Connecting to MQTT..."); //serialdebug
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      //Serial.println("connected");   //serialdebug
    }
    else {
      if (millis() > TIMEOUT) {
        //Serial.println("Timout MQTT connection, going to sleep.."); //serialdebug
        ESP.deepSleepInstant(SLEEPTIME, WAKE_NO_RFCAL);
      }
      //Serial.print("failed with state "); //serialdebug
      //Serial.print(client.state()); //serialdebug
      delay(5);
    }
  }


  //Serial.println("now publishing..."); //serialdebug
  //csv distance and runtime
  sprintf(payload, "%d,%d,%f,%lu", distance,bat_raw, analog_batt, millis());
  client.publish("zisterne/fuellstand", payload);
  delay(10);
  //Serial.print("Payload was: "); //serialdebug
  //Serial.println(payload); //serialdebug
  //Serial.print("Full cycle time: "); //serialdebug
  //Serial.println(millis()); //serialdebug

  //check for updates when esp was manually reset
  //NOTE: reset button must be pressed two times in quick succession, else it will be recognized as deep sleep wake
  /* if (ESP.getResetReason().equals("External System")) { */
  /*   client.publish("zisterne/fuellstand/update", "zisterne is checking for software update"); */
    checkForUpdates(espClient);
    /* ESP.restart();// tells the SDK to reboot, so its a more clean reboot, use this one if possible. */
  /* } */
  //Serial.println("finished, going to sleep.."); //serialdebug
  ESP.deepSleepInstant(SLEEPTIME, WAKE_NO_RFCAL);
  delay(100);
}

void loop(){}
