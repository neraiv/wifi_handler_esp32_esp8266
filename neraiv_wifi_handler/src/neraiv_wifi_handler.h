#if !defined(NERAIV_WIFI_HANDLER_H)
#define NERAIV_WIFI_HANDLER_H

#include <Arduino.h>
#include <string.h>

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif
#if defined(ESP32)
    #include <WiFi.h>
#endif

struct WiFiCredantials{
    String SSID;
    String PASSWORD;
    uint8_t priority;
    bool connectable;
};

class WiFiHandler{

    public:
    	// Pass SSIDs PASSWORDs KnownWiFiCount and ConnectionRequired flag (default true) to class
        WiFiHandler(String SSIDs[],String PASSWORDs[], uint8_t knownWiFiCount, bool WiFiRequired = true);
        
        // Connects to strongest known WiFi found
        bool connectWiFi();            
        
        // Searchs for connectable WiFi and autamticaly sets stongest WiFi priorty first.        
        bool searchIfOneOfKnownWiFiExists();  
        
        // Set WiFi priorty first at given index 
        void setPriorityFirst(uint8_t index);  
        
        // Reconnects WiFi if disconnected, u may want to add this into WiFi.event which handles dissconnect issue if WiFi required.
        bool handleWiFi();
        
    private:
        WiFiCredantials WiFis[5];
        String _SSIDs[5];
        String _PASSWORDs[5];
        uint8_t _knownWiFiCount;
        bool _WiFiRequired;
};


#endif // NERAIV_WIFI_HANDLER_H
