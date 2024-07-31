#include "neraiv_wifi_handler.h"

WiFiHandler::WiFiHandler(const std::vector<WiFi_t*>& wifiList, bool wifi_required)
        : wifiList_(wifiList), _WiFiRequired(wifi_required), lastScanTime(0), scanInterval(60000) {
    WiFi.mode(WIFI_STA); // Set WiFi to station mode (WiFi client)
}


bool WiFiHandler::searchIfOneOfKnownWiFiExists() {
    bool RETURN_VAL = false;
    int dBm = -150;
    
    #ifdef WIFI_HANDLER_DEBUG
    Serial.printf("Scanning for WiFi(s)\n");
    #endif

    int n = WiFi.scanNetworks();

    if (n == 0) {
        #ifdef WIFI_HANDLER_DEBUG
        Serial.println("No networks found.");
        #endif
        return false;
    } else {
        #ifdef WIFI_HANDLER_DEBUG
        Serial.printf("Scan complete; found %d WiFi(s)\n", n);
        #endif
        for (uint8_t i = 0; i < n; i++) {
            #ifdef WIFI_HANDLER_DEBUG
            Serial.printf("%2d", i + 1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.print(" | dBm");
            Serial.printf("%4d", WiFi.RSSI(i));
            Serial.print(" | ch ");
            Serial.printf("%2d", WiFi.channel(i));
            Serial.print(" | ");
            #endif

            bool isKnown = false;
            for (const auto& wifi : wifiList_) {
                if (wifi->ssid_ == WiFi.SSID(i)) {
                    isKnown = true;
                    break;
                }
            }

            #ifdef WIFI_HANDLER_DEBUG
            if (isKnown) {
                Serial.println("KNOWN");
            } else {
                Serial.println("UNKNOWN");
            }
            #endif

            for (uint8_t k = 0; k < wifiList_.size(); k++) {
                if (WiFi.SSID(i) == wifiList_[k]->ssid_) {
                    wifiList_[k]->connectable = true;
                    if (WiFi.RSSI(i) > dBm) {
                        setPriorityFirst(k);
                        dBm = WiFi.RSSI(i);
                    }
                    RETURN_VAL = true;
                }
            }
        }
    }

    WiFi.scanDelete();
    return RETURN_VAL;
}

bool WiFiHandler::connectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
        // Return false to indicate disconnection, handled in handleWiFi
        return false;
    }

    if (searchIfOneOfKnownWiFiExists()) {
        #ifdef WIFI_HANDLER_DEBUG
        Serial.println("Trying to connect to known WiFi...");
        #endif

        for (const auto& wifi : wifiList_) {
            if (wifi->connectable) {
                #if defined(ESP32)
                WiFi.begin(wifi->ssid_.c_str(), wifi->password_.c_str());
                #elif defined(ESP8266)
                WiFi.begin(wifi->ssid_, wifi->password_);
                #endif

                unsigned long startAttemptTime = millis();
                while(WiFi.status() != WL_CONNECTED) {
                    if (millis() - startAttemptTime >= 10000) { // 10 seconds timeout
                        #ifdef WIFI_HANDLER_DEBUG
                        Serial.println("Connection attempt timed out.");
                        #endif
                        return false;
                    }
                    // Non-blocking
                }

                #ifdef WIFI_HANDLER_DEBUG
                Serial.printf("Connected to %s with IP Address ", wifi->ssid_.c_str());
                Serial.println(WiFi.localIP());
                #endif
                return true;
            }
        }
    }

    #ifdef WIFI_HANDLER_DEBUG
    Serial.println("Can't find connectable WiFi");
    #endif
    return false;
}

void WiFiHandler::setPriorityFirst(uint8_t index) {
    if (index >= wifiList_.size()) {
        #ifdef WIFI_HANDLER_DEBUG
        Serial.println("Index out of range");
        #endif
        return;
    }

    WiFi_t* store_wifi = wifiList_[index];

    for (int i = index; i > 0; --i) {
        wifiList_[i] = wifiList_[i - 1];
    }

    wifiList_[0] = store_wifi;
}

bool WiFiHandler::handleWiFi() {
    unsigned long currentMillis = millis();

    // Check if it's time to scan for networks
    if (currentMillis - lastScanTime >= scanInterval) {
        lastScanTime = currentMillis;
        if (WiFi.status() == WL_DISCONNECTED) {
            Serial.println("Hi");
            if (_WiFiRequired) {
                Serial.println("Hioy");
                while (!connectWiFi()) {
                    #ifdef WIFI_HANDLER_DEBUG
                    Serial.println("Retrying again in 1 sec..");
                    #endif
                    delay(1000); // Small delay to avoid flooding the serial output
                }
            } else {
                connectWiFi();
            }
        }
    }
    return false;
}
