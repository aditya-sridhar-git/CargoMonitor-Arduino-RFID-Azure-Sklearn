#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>

#define RST_PIN     22
#define SS_PIN      5
#define DHTPIN      4
#define DHTTYPE     DHT11
#define GAS_SENSOR  34
#define VIBRATION_SENSOR  35

MFRC522 rfid(SS_PIN, RST_PIN); 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  // RFID
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Ready");

  // DHT
  dht.begin();

  // Gas and Vibration
  pinMode(GAS_SENSOR, INPUT);
  pinMode(VIBRATION_SENSOR, INPUT);
}

void loop() {
  // --- RFID ---
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("RFID UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    rfid.PICC_HaltA();
  }

  // --- Temperature and Humidity ---
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" °C | Humidity: "); Serial.print(hum); Serial.println(" %");

  // --- Gas Sensor ---
  int gasValue = analogRead(GAS_SENSOR);
  Serial.print("Gas Level: "); Serial.println(gasValue);

  // --- Vibration Sensor ---
  int vib = analogRead(VIBRATION_SENSOR);
  Serial.print("Vibration Level: "); Serial.println(vib);

  delay(2000); // Delay for readability
}
