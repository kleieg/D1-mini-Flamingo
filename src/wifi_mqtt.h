#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// will be computed as "<HOSTNAME>_<MAC-ADDRESS>"
String Hostname;

int WiFi_reconnect = 0;

// for MQTT
long Mqtt_lastSend = 0;
long lastReconnectAttempt = 0;
int Mqtt_reconnect = 0;

// Initializes the espClient. 
WiFiClient ethClient;
PubSubClient Mqttclient(ethClient);

// Initialize WiFi
void initWiFi() {
    // dynamically determine hostname
  Hostname = HOSTNAME;
  Hostname += "_";
  Hostname += WiFi.macAddress();
  Hostname.replace(":", "");

  WiFi.mode(WIFI_STA);
  WiFi.hostname(Hostname);
  WiFi.begin(ssid, password);
  LOG_PRINT("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    LOG_PRINT(".");
    delay(1000);
  }
  LOG_PRINTF("%s\n",WiFi.localIP().toString().c_str());
}

// reconnect to WiFi 
void reconnect_wifi() {
  LOG_PRINTF("%s\n","WiFi try reconnect"); 
  WiFi_reconnect = WiFi_reconnect + 1;
  WiFi.disconnect();
  WiFi.reconnect();
  delay(500);
  if (WiFi.status() == WL_CONNECTED) {
    // Once connected, publish an announcement...
    LOG_PRINTF("%s\n","WiFi reconnected"); 
  }
}

// This functions reconnects your ESP32 to your MQTT broker

void reconnect_mqtt() {
  String willTopic = Hostname + "/LWT";
  String cmdTopic = Hostname + "/CMD/+";
  if (Mqttclient.connect(Hostname.c_str(), willTopic.c_str(), 0, true, "Offline")) {
    lastReconnectAttempt = 0;
    LOG_PRINTF("%s\n", "connected");

    Mqttclient.publish(willTopic.c_str(), "Online", true);

    Mqttclient.subscribe(cmdTopic.c_str());

    Mqtt_reconnect = Mqtt_reconnect + 1;
  }
}