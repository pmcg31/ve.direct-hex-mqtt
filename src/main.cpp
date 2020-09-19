#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPIFFS.h>
#include "VEDirectTransportSerial.hpp"
#include "VEDirectBMV.hpp"

VEDirectTransportSerial transport(&Serial2);

VEDirectBMV bmv(&transport);

void setup()
{
  if (!SPIFFS.begin(true))
  {
    delay(1000);
    ESP.restart();
  }

  // Load victron defs
  File victronDDFile = SPIFFS.open("/victron_bmv_defs.json", "r");
  if (!victronDDFile)
  {
    Serial.println("Failed to open victron_bmv_defs.json for reading");
    return;
  }
  else
  {
    VEDirectBMV::loadBmvHexDefs(victronDDFile);
  }
  victronDDFile.close();

  // Done with files
  SPIFFS.end();

  Serial2.begin(19200);
}

void loop()
{
  bmv.loop();
}