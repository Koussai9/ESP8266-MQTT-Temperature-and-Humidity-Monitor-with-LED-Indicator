#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char *ssid = "";
const char *password = "";
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
int humidityValue = 0;
int temperatureValue = 0;

int led1 = D5; // Define the pin for LED1
int led2 = D7; // Define the pin for LED2

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  String top = String(topic);
  if(top.equals("Temperature")) {
    temperatureValue = atoi((char*)payload);
    Serial.print("Temperature: ");
    Serial.println(temperatureValue);
  } else if(top.equals("Humidity")) {
    humidityValue = atoi((char*)payload);
    Serial.print("Humidity: ");
    Serial.println(humidityValue);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("temp", "tmp value ");
      // ... and resubscribe
      client.subscribe("Temperature");
      client.subscribe("Humidity");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(led1, OUTPUT); // Set LED1 pin as output
  pinMode(led2, OUTPUT); // Set LED2 pin as output
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check if it's time to check the values
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    // Insert your logic here to check temperature and humidity values
    if (temperatureValue > 30 || humidityValue > 80) {
      digitalWrite(led1, HIGH); // Turn on LED1
      digitalWrite(led2, LOW);  // Turn off LED2
    } else {
      digitalWrite(led1, LOW);  // Turn off LED1
      digitalWrite(led2, HIGH); // Turn on LED2
    }
  }
}
