#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Konfigurasi WiFi
const char* ssid = "Hy";
const char* password = "00000000";

// Variabel untuk sensor kelembapan tanah
int moisture = 0;

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

    // Endpoint untuk kelembapan tanah
    server.on("/moisture", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", String(moisture) + "%");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk menghidupkan pompa
    server.on("/pump/on", HTTP_GET, [](AsyncWebServerRequest *request){
        // Logika menghidupkan pompa
        digitalWrite(2, HIGH); // Sesuaikan dengan pin pompa
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Pump is ON");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk mematikan pompa
    server.on("/pump/off", HTTP_GET, [](AsyncWebServerRequest *request){
        // Logika mematikan pompa
        digitalWrite(2, LOW); // Sesuaikan dengan pin pompa
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Pump is OFF");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk status pompa
    server.on("/pump/status", HTTP_GET, [](AsyncWebServerRequest *request){
        String status = digitalRead(2) == HIGH ? "ON" : "OFF"; // Membaca status pin pompa
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Pump is " + status);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Memulai server
    server.begin();
}

void loop() {
    // Membaca data sensor kelembapan
    moisture = analogRead(A0); // Sesuaikan dengan pin sensor
    delay(1000); // Menunggu 1 detik sebelum membaca lagi
}
