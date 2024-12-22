#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Konfigurasi WiFi
const char* ssid = "Hy";
const char* password = "00000000";

// Pin untuk sensor, relay, LED, dan switch
const int moisturePin = 35;  // Pin untuk sensor kelembapan
const int relayPin = 4;      // Pin untuk relay
const int switchPin = 12;    // Pin untuk switch fisik
const int ledPin = 2;        // Pin untuk LED indikator

// Variabel untuk kelembapan dan mode kontrol
int moisture = 0;
bool manualControl = false;    // Status mode kontrol (true = manual, false = otomatis)
bool relayStatus = false;      // Status relay (true = ON, false = OFF)

// Debouncing untuk switch
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool lastSwitchState = HIGH;
bool currentSwitchState = HIGH;

// Deklarasi server
AsyncWebServer server(80);

void setup() {
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
    pinMode(switchPin, INPUT_PULLUP);  // Pull-up internal untuk switch
    pinMode(ledPin, OUTPUT);

    // Matikan relay dan LED pada awal
    digitalWrite(relayPin, LOW);
    digitalWrite(ledPin, LOW);

    // Endpoint untuk kelembapan tanah
    server.on("/moisture", HTTP_GET, [](AsyncWebServerRequest *request) {
        int moisturePercentage = map(moisture, 4095, 0, 0, 100);
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", String(moisturePercentage));
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk menghidupkan pompa secara manual
    server.on("/pump/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        manualControl = true;
        relayStatus = true;
        digitalWrite(relayPin, HIGH);
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Pump is ON (Manual)");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk mematikan pompa secara manual
    server.on("/pump/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        manualControl = true;
        relayStatus = false;
        digitalWrite(relayPin, LOW);
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Pump is OFF (Manual)");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk mengaktifkan mode otomatis
    server.on("/pump/auto/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        manualControl = false;
        Serial.println("Mode otomatis diaktifkan");
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Auto mode is ON");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk menonaktifkan mode otomatis
    server.on("/pump/auto/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        manualControl = true;
        Serial.println("Mode otomatis dinonaktifkan");
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Auto mode is OFF");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk membaca status mode otomatis
    server.on("/pump/auto/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String mode = manualControl ? "Manual" : "Automatic";
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", mode);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // Endpoint untuk membaca status relay
    server.on("/pump/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String status = relayStatus ? "ON" : "OFF";
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", status);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });


    server.begin();
}

void loop() {
    // Membaca kelembapan
    moisture = analogRead(moisturePin);
    int moisturePercentage = map(moisture, 4095, 0, 0, 100);

    // Pembacaan switch fisik untuk kontrol manual
    currentSwitchState = digitalRead(switchPin);
    if (currentSwitchState != lastSwitchState) {
        // Debouncing
        if (millis() - lastDebounceTime > debounceDelay) {
            if (currentSwitchState == LOW) {  // Saat switch ditekan
                manualControl = true;  // Aktifkan manual mode
                relayStatus = !relayStatus;  // Toggle relay status
                digitalWrite(relayPin, relayStatus ? HIGH : LOW);
                Serial.println(relayStatus ? "Relay ON (Manual)" : "Relay OFF (Manual)");
            }
            lastDebounceTime = millis();
        }
    }
    lastSwitchState = currentSwitchState;

    // Indikator LED untuk mode otomatis/manual
    digitalWrite(ledPin, manualControl ? HIGH : LOW);

    // Kontrol otomatis jika mode otomatis aktif
    if (!manualControl) {
        if (moisturePercentage < 40) {
            digitalWrite(relayPin, HIGH);
            relayStatus = true; // Perbarui status relay
        } else if (moisturePercentage > 60) {
            digitalWrite(relayPin, LOW);
            relayStatus = false; // Perbarui status relay
        }
    }

    // Tampilkan status kelembapan
    Serial.print("Soil Moisture: ");
    Serial.print(moisturePercentage);
    Serial.println("%");

    delay(1000);
}