#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "myheaders.h"
#include "credentials.h"

const int FW_VERSION = 30;
const char* ESP_ID = "210911-zisterne";
const char* fwUrlBase = "http://192.168.178.143/fota/";

// Checks if version in the file http://fwUrlBase/ESP_ID.version" (example: http://192.168.178.143/fota/210822-test.version )
// is never than the number defined in FW_VERSION, if yes it tries to update with the bin file provided at http://fwUrlBase/ESP_ID.bin
void checkForUpdates(WiFiClient espClient) {
  String fwURL = String( fwUrlBase );
  fwURL.concat( ESP_ID );
  String fwVersionURL = fwURL;
  fwVersionURL.concat(".version");

  Serial.println("Checking for firmware updates.");
  Serial.print("Firmware version URL: ");
  Serial.println( fwVersionURL );

  HTTPClient httpClient;
  httpClient.begin(espClient, fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print("Current firmware version: ");
    Serial.println( FW_VERSION );
    Serial.print("Available firmware version: ");
    Serial.println( newFWVersion );

    int newVersion = newFWVersion.toInt();

    if( newVersion > FW_VERSION ) {
      Serial.println("Preparing to update");

      String fwImageURL = fwURL;
      fwImageURL.concat(".bin");

      Serial.print("Trying to get bin at: ");
      Serial.println(fwImageURL);
      /* t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL ); */
      /* t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, "http://192.168.178.143/fota/210822-test.bin");// 80, "fwImageURL"); */
      t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, fwImageURL);// 80, "fwImageURL");
      /* t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, "192.168.178.143", 80, "/fota/210822-test.bin"); */
      /* t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, "192.168.178.143", 80, fwImageURL ); */

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
        case HTTP_UPDATE_OK:
          Serial.println("[update] Update ok."); // may not be called since we reboot the ESP
          break;
      }
    }
    else {
      Serial.println("Already on latest version");
    }
  }
  else {
    Serial.print("Firmware version check failed, got HTTP response code ");
    Serial.println( httpCode );
  }
  httpClient.end();
}
