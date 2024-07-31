#include "WiFi.h"
#include "neraiv_wifi_handler.h"

// Define the number of known WiFi networks (ensure this is consistent with your actual list)
#define WIFI_HANDLER_KNOWN_WIFI_CNT 2

WiFi_t wifi1("Wokwi-GUEST", "");     // Replace with actual credentials if needed
WiFi_t wifi2("Wokwi-GUEST-2", "");   // Replace with actual credentials if needed

std::vector<WiFi_t*> wifiList = { &wifi1, &wifi2 };

WiFiHandler wifiHandler(wifiList, true /* WiFi required, default TRUE */);

void setup() {
  Serial.begin(115200);
  delay(500); // Give time for Serial to initialize
  Serial.println("Setup started");
}

void loop() {
    // Continuously call handleWiFi to manage connection
    wifiHandler.handleWiFi();

    delay(100);
    // Other tasks in your loop
}
