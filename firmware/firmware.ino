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
    wifiMgr.setHostname(hostname);
    wifiMgr.setDarkMode(true);
    wifiMgr.setTitle("Pig-eon 🐖🐦");
    wifiMgr.setDisableConfigPortal(false);
    wifiMgr.setDebugOutput(true);
    wifiMgr.setHttpPort(8080);

    if( wifiMgr.autoConnect(hostname.c_str()) ){ //use hostname as AP name if unable to connect
        DEBUG_PORT.println("WiFiManager connected to WiFi");
        wifiMgr.startWebPortal();
    }
    else {
        DEBUG_PORT.println("WiFiManager could not connect to WiFi, config portal running");
    }

    DEBUG_PORT.println("Registering mDNS hostname: " + hostname + ".local");
    mdns.begin(WiFi.localIP(), hostname.c_str()); //Initiate mDNS and make it reachable with a .local hostname
    
    //Add mDNS service records to make services discoverable
    DEBUG_PORT.println("Registering mDNS Services:");
    mdns.addServiceRecord( String( mdns_description + " Settings._http" ).c_str(), 8080, MDNSServiceTCP);//Publish webserver for management
    DEBUG_PORT.println("✅ Settings Web Server");
    mdns.addServiceRecord( String( mdns_description + " App._http" ).c_str(), 80, MDNSServiceTCP);//Publish webserver for management
    DEBUG_PORT.println("✅ App Web Server");

    mdns.addServiceRecord( String( mdns_description + " PulseAudio Sink._pulse-sink" ).c_str(), 4713, MDNSServiceTCP); //Publish sink for PulseAudio module-zeroconf-discover
    DEBUG_PORT.println("✅ PulseAudio Sink");
    mdns.addServiceRecord( String( mdns_description + " PulseAudio Source._pulse-source" ).c_str(), 4713, MDNSServiceTCP); //Publish source for PulseAudio module-zeroconf-discover
    DEBUG_PORT.println("✅ PulseAudio Source");
}

void loop() {
    wifiMgr.process(); //WiFiManager task

    mdns.run(); //mDNS task
}