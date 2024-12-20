#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h> // Tambahkan library untuk HTTP client

const int soilMoisturePin = 35;  // Pin untuk membaca soil moisture sensor (analog)
const int pumpPin = 4;           // Pin untuk mengendalikan relay pompa (digital)

int moistureValue = 0;           // Variabel untuk menyimpan nilai kelembapan tanah
bool pumpStatus = false;         // Variabel untuk menyimpan status pompa

// Pengaturan jaringan WiFi
const char* ssid = "sabilul";
const char* password = "sabilulesp";

AsyncWebServer server(80); // Membuat server web

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

  // Menambahkan header CORS untuk setiap respons
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

  // Endpoint untuk menangani permintaan OPTIONS
  server.on("/moisture", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    request->send(200);
  });

  server.on("/pump/on", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    request->send(200);
  });

  server.on("/pump/off", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    request->send(200);
  });

  server.on("/pump/status", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    request->send(200);
  });

  // Endpoint untuk mendapatkan data kelembapan
  server.on("/moisture", HTTP_GET, [](AsyncWebServerRequest *request){
    moistureValue = analogRead(soilMoisturePin);
    int moisturePercentage = map(moistureValue, 0, 4095, 0, 100); // Mengonversi nilai kelembapan ke persentase
    String response = String(moisturePercentage) + "%";
    request->send(200, "text/plain", response);  // Mengirimkan respons kelembapan
  });

  // Tambahkan header CORS untuk endpoint lain juga
  server.on("/pump/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(pumpPin, HIGH); // Menghidupkan pompa
    pumpStatus = true;
    request->send(200, "text/plain", "Pump is ON");
  });

  server.on("/pump/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(pumpPin, LOW); // Mematikan pompa
    pumpStatus = false;
    request->send(200, "text/plain", "Pump is OFF");
  });

  server.on("/pump/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String status = pumpStatus ? "ON" : "OFF";
    request->send(200, "text/plain", status);
  });

  // Menangani halaman yang tidak ditemukan
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Not Found"); // Mengirimkan respons 404
  });

  // Memulai server
  server.begin();
}

void loop() {
  // Membaca nilai kelembapan tanah
  moistureValue = analogRead(soilMoisturePin);

  // Menampilkan nilai kelembapan di Serial Monitor
  int moisturePercentage = map(moistureValue, 0, 4095, 0, 100);
  Serial.print("Soil Moisture Percentage: ");
  Serial.print(moisturePercentage);
  Serial.println("%");

  // Kontrol otomatis pompa berdasarkan nilai kelembapan
  if (moisturePercentage < 30) {
    digitalWrite(pumpPin, HIGH);
    pumpStatus = true;
    Serial.println("Pump Status: ON (Auto)");
  } else {
    digitalWrite(pumpPin, LOW);
    pumpStatus = false;
    Serial.println("Pump Status: OFF (Auto)");
  }

  // Mengirim data kelembapan ke server Node.js
  sendMoistureDataToServer(moisturePercentage);

  Serial.println("----------------------------------");
  delay(2000);
}

// Fungsi untuk mengirim data kelembapan ke server Node.js
void sendMoistureDataToServer(int moisturePercentage) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.244.107:3000/moisture"); // Ganti dengan IP server Node.js Anda
    http.addHeader("Content-Type", "application/json");

    String postData = "{\"moisture\": " + String(moisturePercentage) + "}";
    int httpResponseCode = http.POST(postData);

    if(httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Error sending POST request");
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
