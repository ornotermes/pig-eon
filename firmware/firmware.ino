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

String repo_url = "https://github.com/ornotermes/pig-eon";

String mdns_description = "Pig-eon Amateur radio modem (" + id + ")";

const char* get_mdns_txt_str(String device) {
    String records[] = {
        "server-version=pulseaudio 15.0",
        "user-name=root",
        "machine-id=00000000000000000000000000000000",
        "uname=Unknown",
        "fqdn=" + hostname + ".local",
        "cookie=0x00000000",
        "device=" + device,
        "rate=48000",
        "channels=2",
        "format=s16le",
        "channel_map=front-left,front-right",
        "subtype=hardware",
        "description=WiFi Amateur radio modem",
        "vendor-name=" + repo_url,
        "product-name=Pig-eon (" + id + ")",
        "class=sound",
        "form-factor=internal",
        "icon-name=computer-desktop"    
    };
    int records_count = sizeof(records)/sizeof(records[0]); //items in array = size of array in bytes / size of any array item in bytes
    //DEBUG_PORT.println("mDNS txt array count: " + String( records_count ) );
    String str = ""; //formated txt-record string
    for( int i = 0; i < records_count; i++ ){
        str += char( strlen( records[i].c_str() ) ); //length of parameter in bytes as raw char number
        str += records[i]; //parameter text
    }
    return (str.c_str());
}

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

    const char *str = get_mdns_txt_str("output"); //Get a txt record to send audio server properties with mDNS
    //DEBUG_PORT.println(str);
    mdns.addServiceRecord( String( mdns_description + " PulseAudio Sink._pulse-sink" ).c_str(), 4713, MDNSServiceTCP, str); //Publish sink/output for PulseAudio module-zeroconf-discover
    DEBUG_PORT.println("✅ PulseAudio Sink");
    str = get_mdns_txt_str("input");
    mdns.addServiceRecord( String( mdns_description + " PulseAudio Source._pulse-source" ).c_str(), 4713, MDNSServiceTCP, str); //Publish source/input for PulseAudio module-zeroconf-discover
    DEBUG_PORT.println("✅ PulseAudio Source");
}

void loop() {
    wifiMgr.process(); //WiFiManager task

    mdns.run(); //mDNS task
}