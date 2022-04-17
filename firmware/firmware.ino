#include <WiFiManager.h>

//set up some strings depending on the build target
#if defined(ESP32)
    String id = String((uint32_t)ESP.getEfuseMac(), HEX);
    String hostname = "pig-eon_" + id;
#else
    String serial = "0";
    String hostname = "pig-eon";
#endif

WiFiManager wifiMgr; //WiFiManager manages connections to wireless networks

void setup() {
    Serial.begin(115200); //Set up serial debug console

    WiFi.mode(WIFI_STA); //Set WiFi to STA/Client mode

    //wifiMgr.resetSettings(); //Forget configured wifi settings

    wifiMgr.setConfigPortalBlocking(false); //Run WiFiManager as non-blocking

    bool res;
    if( wifiMgr.autoConnect(hostname.c_str()) ){ //use hostname as AP name if unable to connect
        Serial.println("WiFiManager connected to WiFi");
    }
    else {
        Serial.println("WiFiManager could not connect to WiFi, config portal running");
    }
}

void loop() {
    wifiMgr.process();
}