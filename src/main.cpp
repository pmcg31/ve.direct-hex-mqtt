#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h>
#include <Adafruit_Si7021.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include "config.hpp"
#include "mqtt_discovery.hpp"
#include "VEDirectTransportSerial.hpp"
#include "VEDirectBMV.hpp"

VEDirectTransportSerial transport(&Serial2);

VEDirectBMV bmv(&transport);

AsyncMqttClient mqttClient;
const uint16_t discoveryPort = 2112;
const uint16_t localPort = 2113;
MQTTDiscovery mqttDiscovery(discoveryPort,
                            localPort,
                            &mqttClient);

Adafruit_Si7021 tempHumSensor;

void doTempHumSensor();

int getStepPrecision(const char *step);

const int reportRate_ms = 1000;
unsigned long nextThingMillis;

Config config;

char customName[33];

const char *fieldsToMonitor[] = {
    "mainVoltage",
    "hiResCurrent",
    "power",
    "consumedAh",
    "soc",
    "ttg",
    "syncState",
    "uptime",
    ""};

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin(true))
  {
    Serial.println("Failed to start SPIFFS");
    delay(1000);
    ESP.restart();
  }

  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile)
  {
    delay(1000);
    ESP.restart();
  }

  if (!config.readConfig(configFile))
  {
    delay(1000);
    ESP.restart();
  }
  configFile.close();

  WiFi.mode(WIFI_STA);
  WiFi.begin(config.getSSID(), config.getKey());
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    delay(1000);
    ESP.restart();
  }

  ArduinoOTA.setHostname(config.getMDNS());
  ArduinoOTA.begin();

  tempHumSensor = Adafruit_Si7021();
  if (!tempHumSensor.begin())
  {
    delay(1000);
    ESP.restart();
  }

  mqttDiscovery.discoverAndConnectBroker();

  // Load victron defs
  File victronDDFile = SPIFFS.open("/victron_bmv_defs.json", "r");
  if (!victronDDFile)
  {
    Serial.println("Failed to open 'victron_bmv_defs.json' for reading");
    return;
  }
  else
  {
    if (!VEDirectBMV::loadBmvHexDefs(victronDDFile))
    {
      Serial.printf("Failed to load victron hex defs: %s", VEDirectBMV::getLoadBmvHexDefsError());
    }
    else
    {
      Serial.println("Victron hex defs loaded!");
    }
  }
  victronDDFile.close();

  // Done with files
  SPIFFS.end();

  Serial2.begin(19200);

  VEDirectBMVProductIdResp resp = bmv.productId();
  if (resp.isError())
  {
    Serial.printf("Failed to get product id: %s\n", resp.getErrorMessage());
  }
  else
  {
    Serial.printf("Product id: 0x%04X\n", resp.getProductId());
    Serial.printf("Product name: %s\n", resp.getProductName());
  }

  VEDirectHexFieldStringResp resp2 = bmv.getStringByName("description");
  if (resp2.isError())
  {
    Serial.printf("Failed to get description: %s\n", resp2.getMessage());
  }
  else
  {
    strcpy(customName, resp2.getValue());
    Serial.printf("Product description: %s\n", customName);
  }

  nextThingMillis = millis() + reportRate_ms;
}

void loop()
{
  DynamicJsonDocument doc(4096);
  char json[1024];
  char topic[500];
  char tmp[50];

  ArduinoOTA.handle();

  unsigned long now = millis();

  if (now >= nextThingMillis)
  {
    nextThingMillis = now + reportRate_ms;

    doTempHumSensor();

    int fieldIdx = 0;
    const char *fieldName = fieldsToMonitor[fieldIdx++];
    int precision = 0;
    while (strcmp(fieldName, "") != 0)
    {
      VEDirectHexFieldResp resp = bmv.getByName(fieldName);
      if (resp.isError())
      {
        Serial.printf("Error getting '%s'': %s\n", fieldName, resp.getMessage());
      }
      else
      {
        JsonObject fieldInfo = resp.getFieldInfo();
        const char *storage = fieldInfo["storage"];

        if (strcmp(storage, "un8") == 0)
        {
          uint8_t value = resp.getValue().asUn8();

          const char *stepCh = fieldInfo["step"];
          precision = getStepPrecision(stepCh);
          double step = strtod(stepCh, 0);

          double dValue = (double)value * step;
          sprintf(tmp, "%.*f", precision, dValue);
        }
        else if (strcmp(storage, "sn16") == 0)
        {
          int16_t value = resp.getValue().asSn16();

          const char *stepCh = fieldInfo["step"];
          precision = getStepPrecision(stepCh);
          double step = strtod(stepCh, 0);

          double dValue = (double)value * step;
          sprintf(tmp, "%.*f", precision, dValue);
        }
        else if (strcmp(storage, "un16") == 0)
        {
          uint16_t value = resp.getValue().asUn16();

          const char *stepCh = fieldInfo["step"];
          precision = getStepPrecision(stepCh);
          double step = strtod(stepCh, 0);

          double dValue = (double)value * step;
          sprintf(tmp, "%.*f", precision, dValue);
        }
        else if (strcmp(storage, "un24") == 0)
        {
          uint32_t value = resp.getValue().asUn24();

          const char *stepCh = fieldInfo["step"];
          precision = getStepPrecision(stepCh);
          double step = strtod(stepCh, 0);

          double dValue = (double)value * step;
          sprintf(tmp, "%.*f", precision, dValue);
        }
        else if (strcmp(storage, "sn32") == 0)
        {
          int32_t value = resp.getValue().asSn32();

          const char *stepCh = fieldInfo["step"];
          precision = getStepPrecision(stepCh);
          double step = strtod(stepCh, 0);

          double dValue = (double)value * step;
          sprintf(tmp, "%.*f", precision, dValue);
        }
        else if (strcmp(storage, "un32") == 0)
        {
          uint32_t value = resp.getValue().asUn32();

          const char *stepCh = fieldInfo["step"];
          precision = getStepPrecision(stepCh);
          double step = strtod(stepCh, 0);

          double dValue = (double)value * step;
          sprintf(tmp, "%.*f", precision, dValue);
        }
        else
        {
          Serial.printf("Unknown storage type: %s", storage);
        }

        bool jsonOutput = true;
        if (jsonOutput)
        {
          doc["value"] = tmp;
          doc["unit"] = fieldInfo["unit"];
          serializeJsonPretty(doc, json);
        }
        else
        {
          sprintf(json, "%s %s", tmp, fieldInfo["unit"].as<char *>());
        }

        if (mqttClient.connected())
        {
          sprintf(topic, "pmcg-esp32/%s/%s/%s", customName, fieldInfo["group"].as<char *>(), fieldName);
          mqttClient.publish(topic, 0, false, json, strlen(json));
        }

        // Serial.printf("Current: %.03f A\n", tmp);
      }

      fieldName = fieldsToMonitor[fieldIdx++];
    }
  }
}

void doTempHumSensor()
{
  if (mqttClient.connected())
  {
    char buf[20];

    sprintf(buf, "%.02f", tempHumSensor.readHumidity());
    mqttClient.publish("pmcg-esp32/humidity", 0, false, buf, strlen(buf));

    sprintf(buf, "%.02f", tempHumSensor.readTemperature());
    mqttClient.publish("pmcg-esp32/temperature", 0, false, buf, strlen(buf));
  }
}

int getStepPrecision(const char *step)
{
  bool foundPeriod = false;
  int precision = 0;

  for (const char *tmp = step; *tmp != 0; tmp++)
  {
    if (*tmp == '.')
    {
      foundPeriod = true;
    }
    else
    {
      if (foundPeriod)
      {
        precision++;
      }
    }
  }

  return precision;
}
