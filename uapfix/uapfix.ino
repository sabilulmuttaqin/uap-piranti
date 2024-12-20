#include <WiFi.h>
#include <HTTPClient.h>

const int soilMoisturePin = 35;  // Pin untuk membaca soil moisture sensor (analog)
const int pumpPin = 4;           // Pin untuk mengendalikan relay pompa (digital)

int moistureValue = 0;           // Variabel untuk menyimpan nilai kelembapan tanah
bool pumpStatus = false;         // Variabel untuk menyimpan status pompa

// Pengaturan jaringan WiFi
const char* ssid = "sabilul";
const char* password = "sabilulesp";

// Server URL di Laragon
const String serverUrl = "http://localhost:3000";

void setup() {
  Serial.begin(115200);     // Memulai komunikasi serial
  pinMode(pumpPin, OUTPUT);
  WiFi.begin(ssid, password);

  // Menunggu koneksi WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Membaca nilai kelembapan tanah
  moistureValue = analogRead(soilMoisturePin);

  // Menampilkan nilai kelembapan di Serial Monitor
  Serial.print("Soil Moisture Value (Raw): ");
  Serial.println(moistureValue);

  int moisturePercentage = map(moistureValue, 0, 4095, 0, 100);
  Serial.print("Soil Moisture Percentage: ");
  Serial.print(moisturePercentage);
  Serial.println("%");

  // Kontrol otomatis pompa berdasarkan nilai kelembapan
  if (moistureValue < 1500) {
    digitalWrite(pumpPin, HIGH);
    pumpStatus = true;
    Serial.println("Pump Status: ON (Auto)");
  } else {
    digitalWrite(pumpPin, LOW);
    pumpStatus = false;
    Serial.println("Pump Status: OFF (Auto)");
  }

  // Mengirimkan status kelembapan ke server
  HTTPClient http;
  http.begin(serverUrl + "/moisture");
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Soil Moisture from Server: " + payload);
  } else {
    Serial.println("Failed to connect to server");
  }

  http.end();

  // Menunggu beberapa detik sebelum pembacaan berikutnya
  delay(2000);
}
