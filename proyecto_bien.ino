#include <FS.h>                
#include <ESP8266WiFi.h> 
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         
#include <ArduinoJson.h>          
char blynk_token[34] = "ldiI6s9Y_tV-jTymTUJzOtQ4IoDVQZ1p"; //El comando char se utiliza para almacenar varios caracteres 
bool shouldSaveConfig = false;//El comando bool se utiliza cuando solo se obtendran dos posibles valores
void saveConfigCallback () {
Serial.println("guardar la configuracion");
      shouldSaveConfig = true;
    }
    

void setup() {

      Serial.begin(9600);
      Serial.println();
      Serial.println("Configuracion FS...");

      if (SPIFFS.begin()) {
        Serial.println("Archivos escritos");
        if (SPIFFS.exists("/config.json")) {
          Serial.println("Leyendo conf");
          File configFile = SPIFFS.open("/config.json", "r");
          if (configFile) {
            Serial.println("Archivo abierto");
            size_t size = configFile.size();
            // Allocate a buffer to store contents of the file.
            std::unique_ptr<char[]> buf(new char[size]);

            configFile.readBytes(buf.get(), size);
            DynamicJsonBuffer jsonBuffer;
            JsonObject& json = jsonBuffer.parseObject(buf.get());
            json.printTo(Serial);
            if (json.success()) {
              Serial.println("\nparsed json");
              strcpy(blynk_token, json["blynk_token"]);
    
            } else {
              Serial.println("Fallo conf json");
            }
          }
        }
      } else {
        Serial.println("Fallo conexion");
      }
      WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 33);
      
      WiFiManager wifiManager;
      wifiManager.setSaveConfigCallback(saveConfigCallback);
      wifiManager.addParameter(&custom_blynk_token);
        wifiManager.autoConnect("Yuyay-Switch");
      if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
        Serial.println("Fallo conexion, timeout");
        delay(3000);
        ESP.reset();
        delay(5000);
      }
      Serial.println("Conectado si :)");
      strcpy(blynk_token, custom_blynk_token.getValue());
      if (shouldSaveConfig) {
        Serial.println("saving config");
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        json["blynk_token"] = blynk_token;

        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) {
          Serial.println("failed to open config file for writing");
        }

        json.printTo(Serial);
        json.printTo(configFile);
        configFile.close();
      }

      Serial.println("local ip");
      Serial.println(WiFi.localIP());
      Blynk.config(blynk_token);
      Blynk.begin(blynk_token, WiFi.SSID().c_str(), WiFi.psk().c_str());
    }

    void loop() {

      Blynk.run();
    }
