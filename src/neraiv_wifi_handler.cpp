#include "neraiv_wifi_handler.h"



WiFiHandler::WiFiHandler(String SSIDs[],String PASSWORDs[], uint8_t knownWiFiCount, bool WiFiRequired){
    for(uint8_t i = 0; i<knownWiFiCount; i++){
        WiFis[i].SSID = SSIDs[i];
        WiFis[i].PASSWORD = PASSWORDs[i];
        WiFis[i].priority = i;
    } 
    _knownWiFiCount = knownWiFiCount;

    _WiFiRequired = WiFiRequired; 
}

/* bool WiFiHandler::createAccsessPointWithStaticIP(String SSID, String PASSWORD, IPAddress ip, IPAddress subnet){
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip,ip,subnet);
    WiFi.softAP(SSID,PASSWORD);
    Serial.println(WiFi.localIP());
    return false;
} */

bool WiFiHandler::searchIfOneOfKnownWiFiExists(){

    bool RETURN_VAL = false;
    int dBm = -150;
    Serial.printf("Scaning for WiFi(s)\n");

    int n = WiFi.scanNetworks();

    Serial.printf("Scan complate found %d WiFi(s)\n",n);


    if( n == 0){
        return false;
    }else{
        for(uint8_t i = 0;  i<n;i++){
            Serial.printf("%2d",i + 1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.print(" | dBm");
            Serial.printf("%4d", WiFi.RSSI(i));
            Serial.print(" | ");
            Serial.printf("%2d", WiFi.channel(i));
            Serial.print(" | \n");
            for(uint8_t k = 0; k<_knownWiFiCount; k++){
                if(WiFi.SSID(i) == WiFis[k].SSID){
                    WiFis[k].connectable = true;
                    if(WiFi.RSSI(i) < dBm){
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

bool WiFiHandler::connectWiFi(){
    WiFi.disconnect();
    delay(500);
    if(searchIfOneOfKnownWiFiExists()){
        for(uint8_t i = 0; i<_knownWiFiCount;i++){
            if(WiFis[i].connectable){
#if defined(ESP32)
                WiFi.begin(WiFis[i].SSID.c_str(), WiFis[i].PASSWORD.c_str());
                delay(500);
                Serial.printf("Connected to %s with IP Address ", WiFis[i].SSID);
                Serial.println(WiFi.localIP());
                return true;
#endif
#if defined(ESP8266)
                WiFi.begin(WiFis[i].SSID, WiFis[i].PASSWORD);
                delay(500);
                Serial.printf("Connected to %s with IP Address ", WiFis[i].SSID);
                Serial.println(WiFi.localIP());
                return true;
#endif          
            }
        }
    }
    Serial.println("Cant find Connectable WiFi");

    return false;
}

void WiFiHandler::setPriorityFirst(uint8_t index){
    String store_SSID = WiFis[index].SSID;
    String store_PASSWORD = WiFis[index].PASSWORD;
    bool   store_connactable = WiFis[index].connectable;


    for(index; index>=1; index--){
        WiFis[index].SSID = WiFis[index -1].SSID;
        WiFis[index].PASSWORD = WiFis[index -1].PASSWORD;
        WiFis[index].connectable = WiFis[index -1].connectable;
    }

    WiFis[0].SSID = store_SSID;
    WiFis[0].PASSWORD = store_PASSWORD;
    WiFis[0].connectable = store_connactable;
}   


bool WiFiHandler::handleWiFi(){
    if(WiFi.status() == WL_DISCONNECTED){
        if(_WiFiRequired){
            
            while (!connectWiFi())
            {
                Serial.println("");
                Serial.println("Retrying again in 5 seconds");
                delay(5000);
            }      
        } else {
            connectWiFi();
        }
    }
    return false;
}
