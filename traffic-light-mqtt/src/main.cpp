#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "ArduinoJson-v6.11.0.h"

//define buttons
#define BTNLEFT D4
#define BTNRIGHT D5

//define LEDS left
#define LRED D1
#define LYELLOW D2
#define LGREEN D3

//define LEDS right
#define RRED D0
#define RYELLOW D7
#define RGREEN D6

//Timer variables
#define YELLOWTIME 2000
#define TIMERTIME 5000

// CONFIG WIFI
const char *ssid = "1";
const char *password = "12345678";

//CONFIG MQTT
const char *mqtt_broker = "test.mosquitto.org";
int mqtt_port = 1883;
const char *mqtt_topic = "trafficlight";
const char *mqtt_username = "usr";
const char *mqtt_password = "pannekoek";

int timer = TIMERTIME;

WiFiClient client;

PubSubClient mqttClient("", 0, client);

bool leftActive = false;
bool rightActive = false;

void clearAllLEDs()
{
  digitalWrite(RRED, LOW);
  digitalWrite(RYELLOW, LOW);
  digitalWrite(RGREEN, LOW);
  digitalWrite(LRED, LOW);
  digitalWrite(LYELLOW, LOW);
  digitalWrite(LGREEN, LOW);
}

void SwitchToLeft()
{
  if (!leftActive)
  {
    leftActive = true;
    rightActive = false;
    clearAllLEDs();
    digitalWrite(LRED, HIGH);
    digitalWrite(RYELLOW, HIGH);
    delay(YELLOWTIME);
    digitalWrite(RYELLOW, LOW);
    digitalWrite(RRED, HIGH);
    digitalWrite(LRED, LOW);
    digitalWrite(LGREEN, HIGH);
    timer = TIMERTIME;
  }
}

void switchToRight()
{
  if (!rightActive)
  {
    leftActive = false;
    rightActive = true;
    clearAllLEDs();
    digitalWrite(LYELLOW, HIGH);
    digitalWrite(RRED, HIGH);
    delay(YELLOWTIME);
    digitalWrite(LYELLOW, LOW);
    digitalWrite(LRED, HIGH);
    digitalWrite(RGREEN, HIGH);
    digitalWrite(RRED, LOW);
    timer = TIMERTIME;
  }
}

void switchToRed()
{
  leftActive = false;
  rightActive = false;
  clearAllLEDs();
  digitalWrite(RRED, HIGH);
  digitalWrite(LRED, HIGH);
}

void waitState()
{
  while (timer > 1)
  {
    timer--;
    delay(1);
  }
}

//kijkt voor nieuwe berichten in de mqtt server en parced de berichten voor ons
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{

  Serial.println("\n\nA new mqtt message");

  if (0 == strcmp(topic, mqtt_topic))
  {
    DynamicJsonDocument jsonDocument(1024);

    DeserializationError error = deserializeJson(jsonDocument, payload);
    if (!error)
    {
      if (strcmp(jsonDocument["state"],"left"))
      {
        waitState();
        SwitchToLeft();
      }

      if (strcmp(jsonDocument["state"], "right"))
      {
        waitState();
        switchToRight();
      }
    }
    else
    {
      Serial.print("This message cant be parced here.");
    }
  }
  else
  {
    Serial.println("error reading message");
  }
}

//verbind de esp met de mqttserver
void mqtt_connect()
{
  mqttClient.setClient(client);
  mqttClient.setServer(mqtt_broker, mqtt_port);

  // Connect with unique id
  String clientId = "verkeerslicht-";
  clientId += String(random(0xffff), HEX);

  if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password))
  {

    // Subscribe to topic
    mqttClient.subscribe(mqtt_topic);

    // Setup callback
    mqttClient.setCallback(mqtt_callback);
    Serial.printf("%s: Connected to %s:%d\n", __FUNCTION__, mqtt_broker, mqtt_port);
  }
  else
  {
    Serial.printf("%s: Connection ERROR (%s:%d)\n", __FUNCTION__, mqtt_broker, mqtt_port);
    delay(2000);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(D5, INPUT);
  pinMode(D4, INPUT);
  pinMode(LRED, OUTPUT);
  pinMode(LYELLOW, OUTPUT);
  pinMode(LGREEN, OUTPUT);
  pinMode(RRED, OUTPUT);
  pinMode(RYELLOW, OUTPUT);
  pinMode(RGREEN, OUTPUT);

  clearAllLEDs();
  switchToRed();

  // Enable WiFi and wait for connection
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print("");
  Serial.println("Wifi Connected: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!mqttClient.connected())
    {
      mqtt_connect();
    }
    else
    {
      mqttClient.loop();
    }
  }
  else
  {
    Serial.println("Geen WiFi verbinding !");
    delay(900);
  }
  timer--;
  delay(1);
}