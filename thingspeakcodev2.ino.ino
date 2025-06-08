#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include <MFRC522.h>

//ThingSpeak credentials
const char* ssid = "YOUR WIFI SSID"; // users service set identifier
const char* password = "YOUR WIFI PASSWORD"; //users wifi password 

unsigned long channelid = 0; // your thingspeak channel id (from thingspeak channel settings) - should be a number
const char* writeapikey = "YOUR THINGSPEAK WRITE API KEY"; // your thingspeak write API key (from thingspeak channel settings)

int tempField = 1;
int humidityField = 2;
int smokeField = 3;
int vibrationField = 4;

WiFiClient client;

//Input Pins
#define DHTPIN 27        // DHT11 data pin connected to GPIO27
#define DHTTYPE DHT11    // DHT 11
#define MQ135_PIN 34     // MQ135 analog output connected to GPIO34 (ADC)
#define SW420_PIN 26     // SW420 digital output connected to GPIO26

//Output Pins
#define LEDPIN 2         // Changed from invalid syntax
#define BUZZER 4         // Changed from invalid syntax

// DHT Object initialisation
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LEDPIN, OUTPUT);    // Fixed typo: OUPUT -> OUTPUT
  pinMode(BUZZER, OUTPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(MQ135_PIN, INPUT);
  pinMode(SW420_PIN, INPUT);

  WiFi.begin(ssid, password);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count <= 20) {  // Fixed: added WL_CONNECTED and missing semicolon
    Serial.println("Connecting...");
    delay(1000);  // Added delay to prevent rapid looping
    count++;
  }
  
  if (count > 20) {
    Serial.println("Connection Failed, try again");
  }
  else {
    Serial.println("Connection was Successful, you may proceed");
    ThingSpeak.begin(client);
  }
}

void loop() {
  // Read Temperature and Humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read MQ135 analog value (for H2S and other gases)
  int gasValue = analogRead(MQ135_PIN);

  // Read SW420 vibration sensor (digital)
  int vibration = digitalRead(SW420_PIN);

  // Output to Serial Monitor
  Serial.println("----- Sensor Data -----");
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT11 sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    ThingSpeak.setField(tempField, temperature);
    ThingSpeak.setField(humidityField, humidity);
  }

  Serial.print("Gas Sensor (MQ135) Value: ");
  Serial.println(gasValue);
  ThingSpeak.setField(smokeField, gasValue);

  Serial.print("Vibration Detected: ");
  Serial.println(vibration == HIGH ? "YES" : "NO");
  ThingSpeak.setField(vibrationField, vibration);  // Fixed: added missing parameter

  int status = ThingSpeak.writeFields(channelid, writeapikey);
  Serial.println(status == 200 ? "Data Sent" : "Data Could not be sent");  // Fixed: printlln -> println, = -> ==

  delay(15000); // Delay before next read
}
