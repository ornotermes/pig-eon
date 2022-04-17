#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <MDNS_Generic.h>

#define DEBUG_PORT Serial

//set up some strings depending on the build target
#if defined(ESP32)
    String id = String((uint32_t)ESP.getEfuseMac(), HEX);
    String hostname = "pig-eon_" + id;
#else
    String id = "0";
    String hostname = "pig-eon";
#endif

String mdns_description = "Pig-eon Amateur radio modem (" + id + ")";

WiFiManager wifiMgr; //WiFiManager manages connections to wireless networks

WiFiUDP udp; //Construct UDP
MDNS mdns(udp); //Construct mDNS

void setup() {
    DEBUG_PORT.begin(115200); //Set up serial debug console

    WiFi.mode(WIFI_STA); //Set WiFi to STA/Client mode

    //wifiMgr.resetSettings(); //Forget configured wifi settings

    wifiMgr.setConfigPortalBlocking(false); //Run WiFiManager as non-blocking

    bool res;
    if( wifiMgr.autoConnect(hostname.c_str()) ){ //use hostname as AP name if unable to connect
        DEBUG_PORT.println("WiFiManager connected to WiFi");
    }
    else {
        DEBUG_PORT.println("WiFiManager could not connect to WiFi, config portal running");
    }

    DEBUG_PORT.println("Registering mDNS hostname: " + hostname);
    mdns.begin(WiFi.localIP(), hostname.c_str()); //Initiate mDNS and make it reachable with a .local hostname
    
    //Add mDNS service records to make services discoverable
    mdns.addServiceRecord( String( mdns_description + " Web Server._http" ).c_str(), 80, MDNSServiceTCP);
}

void loop() {
    wifiMgr.process(); //WiFiManager task

    mdns.run(); //mDNS task
}