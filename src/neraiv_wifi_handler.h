#if !defined(NERAIV_WIFI_HANDLER_H)
#define NERAIV_WIFI_HANDLER_H

#include <vector>
#include <Arduino.h>
#include <string.h>

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif
#if defined(ESP32)
    #include <WiFi.h>
#endif

// Uncomment the following line to enable debug messages
#define WIFI_HANDLER_DEBUG

enum class WiFiError {
    SetupParametersDoesNotMatch,
    CantConnectWiFi,
    CantRetrieveIpAddress
};

class WiFi_t {
public:
    WiFi_t(const String& ssid, const String& password)
        : ssid_(ssid), password_(password), priority(0), connectable(false) {}

    String ssid_;
    String password_;
    uint8_t priority;
    bool connectable;
};

class WiFiHandler {
public:
    WiFiHandler(const std::vector<WiFi_t*>& wifiList, bool wifi_required=true);

    bool connectWiFi();
    bool searchIfOneOfKnownWiFiExists();
    bool handleWiFi();

private:
    void setPriorityFirst(uint8_t index);

    std::vector<WiFi_t*> wifiList_;
    int _selected_wifi_index = 0;
    bool _WiFiRequired;
    
    unsigned long lastScanTime;
    const unsigned long scanInterval;
};

#endif // NERAIV_WIFI_HANDLER_H
