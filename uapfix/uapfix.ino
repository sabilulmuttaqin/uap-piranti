#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Konfigurasi WiFi
const char* ssid = "Hy";
const char* password = "00000000";

// Pin untuk sensor kelembapan dan relay
const int moisturePin = 35;  // Pin untuk sensor kelembapan
const int relayPin = 4;      // Pin untuk relay

// Variabel untuk sensor kelembapan tanah
int moisture = 0;

// Status mode kontrol (true = manual, false = otomatis)
bool manualControl = false;

// Deklarasi server
AsyncWebServer server(80);

void setup() {
    // Memulai Serial Monitor
    Serial.begin(115200);

    // Menghubungkan ke WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());

    // Konfigurasi pin
    pinMode(moisturePin, INPUT);
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); // Pastikan pompa mati di awal

    // Endpoint untuk kelembapan tanah
    server.on("/moisture", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Konversi nilai analog ke persen (0-100%)
    int moisturePercentage = map(moisture, 4095, 0, 0, 100);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", String(moisturePercentage));
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
});


    // Endpoint untuk menghidupkan pompa secara manual
    server.on("/pump/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        manualControl = true; // Aktifkan mode manual
        digitalWrite(relayPin, HIGH); // Hidupkan pompa
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Pump is ON (Manual)");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk mematikan pompa secara manual
    server.on("/pump/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        manualControl = true; // Aktifkan mode manual
        digitalWrite(relayPin, LOW); // Matikan pompa
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Pump is OFF (Manual)");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk mengaktifkan auto mode
    server.on("/pump/auto/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        manualControl = false;
        Serial.println("Mode otomatis diaktifkan");
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Auto mode is ON");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk menonaktifkan auto mode
    server.on("/pump/auto/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        manualControl = true;
        Serial.println("Mode otomatis dinonaktifkan");
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Auto mode is OFF");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk membaca status auto mode
    server.on("/pump/auto/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String mode = manualControl ? "Manual" : "Automatic";
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", mode);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });


    // Endpoint untuk status pompa
    server.on("/pump/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String status = digitalRead(relayPin) == HIGH ? "ON" : "OFF"; // Membaca status pin pompa
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", " " + status);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Memulai server
    server.begin();
}

void loop() {
    // Membaca data kelembapan
    moisture = analogRead(moisturePin);
    int moisturePercentage = map(moisture, 4095, 0, 0, 100); // Konversi ke persen (0-100)

    // Jika dalam mode otomatis, kontrol pompa berdasarkan kelembapan
    if (!manualControl) {
        if (moisturePercentage < 40) { // Jika kelembapan di bawah 30%, nyalakan pompa
            digitalWrite(relayPin, HIGH);
        } else if (moisturePercentage > 60) { // Jika kelembapan di atas 70%, matikan pompa
            digitalWrite(relayPin, LOW);
        }
    }

    // Cetak data kelembapan ke Serial Monitor
    Serial.print("Soil Moisture: ");
    Serial.print(moisturePercentage);
    Serial.println("%");

    // Tunggu 1 detik sebelum membaca lagi
    delay(1000);
}
