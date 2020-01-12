/* 
 * This file is part of the ESP32Clock distribution (https://github.com/zebrajaeger/Esp32Clock).
 * Copyright (c) 2019 Lars Brandt.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* #region  includes */
#include <Arduino.h>
#include <ESPmDNS.h>
#include <Esp.h>
#include <WebServer.h>
#include <WiFi.h>

#include <AutoConnect.h>
#include <U8g2lib.h>
#include <ezTime.h>

#include "net/ota.h"
#include "statistic.h"
#include "util/logger.h"
#include "util/nvs.h"
#include "util/reset.h"
#include "util/utils.h"
#include "wifiutils.h"
/* #endregion */

/* #region  Variables */
Logger LOG("MAIN");
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 33, 32, /* reset=*/U8X8_PIN_NONE);
Timezone myTimezone;
OTA ota;
Statistic statistics;
WebServer webServer;
AutoConnect autoConnect(webServer);
NVS nvs("storage");
Reset reset;

String timezone = "Europe/Berlin";
String currentIP;
String id;
EspClass esp;

bool connected = false;
enum { STATE_BOOT = 0, STATE_BOOT_DONE, STATE_HAS_NTP_TIME, STATE_HAS_TIMEZONE, STATE_NO_TIMEZONE } state;
/* #endregion */

/* #region  Resources */
extern const char configServerMenu[] asm("_binary_configserver_menu_json_start");
/* #endregion */

/* #region  Constants */
#define NVS_DEVICENAME "devicename"
#define NVS_TIMEZONE "timezone"

#define ROOT "/"
#define AC_ROOT "/_ac"

#define AC_DEVICE_SECTION "/device"
#define AC_DEVICE_SECTION_SET "/device_set"
#define AC_DEVICE_SECTION_DEVICENAME "devicename"

#define AC_TIMEZONE_SECTION "/timezone"
#define AC_TIMEZONE_SECTION_SET "/timezone_set"
#define AC_TIMEZONE_SECTION_TIMEZONE "timezone"

#define AC_FACTORYRESET_SECTION "/factory_reset"
#define AC_FACTORYRESET_SECTION_SET "/factory_reset_set"
#define AC_FACTORYRESET_SECTION_SURE "sure"
/* #endregion */

/* #region  Predeclarations */
void setup();
void loop();
bool webserverGetParameter(const String& key, String& result);
bool autoconfigSet(const String& section, const String& name, const String& value);
void setMDNSName(const String name);
void redirect(const String toLocation);
const char* getWifiEventName(WiFiEvent_t e);
const char* getWifiFailReason(uint8_t r);
void showBootScreen();
void showConnectScreen();
void showConnectionFailed(uint8_t reason);
void showTime();
void factoryReset();
/* #endregion */

/* #region setupDetails */
// --------------------------------------------------------------------------------
void setupDisplay()
// --------------------------------------------------------------------------------
{
  u8g2.begin();
}

// --------------------------------------------------------------------------------
void setupSerial()
// --------------------------------------------------------------------------------
{
  // Serial
  Serial.begin(115200);
}

// --------------------------------------------------------------------------------
void setupNVS()
// --------------------------------------------------------------------------------
{
  // NVS Storage
  if (nvs.begin()) {
    LOG.i("Storage initialized");
  } else {
    LOG.e("Storage not initialized");
  }

  //         ID / name
  if (nvs.readString(NVS_DEVICENAME, id)) {
    LOG.i("Got devicename from nvs.");
  } else {
    id = Utils::createId();
    LOG.w("Could not read devicename from nvs. Using generated");
  }
  LOG.i("ID: '%s'", id.c_str());

  //         Timezone
  if (nvs.readString(NVS_TIMEZONE, timezone)) {
    LOG.i("Got timezone from nvs.");
  } else {
    id = Utils::createId();
    LOG.w("Could not read timezone from nvs. Using default");
  }
  LOG.i("TIMEZONE: '%s'", timezone.c_str());
}

// --------------------------------------------------------------------------------
void setupDNS()
// --------------------------------------------------------------------------------
{
  // DNS
  setMDNSName(id);
}

// --------------------------------------------------------------------------------
void setupWiFi()
// --------------------------------------------------------------------------------
{
  // WiFI
  WiFi.begin();

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    LOG.i("WiFi event: %u -> %s\n", event, getWifiEventName(event));
    switch (event) {
      case SYSTEM_EVENT_STA_GOT_IP:
        LOG.i("WiFi connected");
        currentIP = WiFi.localIP().toString();
        LOG.i("IP is: %s", currentIP.c_str());
        connected = true;
        break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
        showConnectionFailed(info.disconnected.reason);
        LOG.i("WiFi disconnected, Reason: %u -> %s\n", info.disconnected.reason, getWifiFailReason(info.disconnected.reason));
        currentIP = "<disconnected>";
        connected = false;
        if (info.disconnected.reason == 202) {
          LOG.i("WiFi Bug, REBOOT/SLEEP!");
          esp_sleep_enable_timer_wakeup(10);
          esp_deep_sleep_start();
          delay(100);
        }
        break;
      default:
        break;
    }
  });
}

// --------------------------------------------------------------------------------
void setupOTA()
// --------------------------------------------------------------------------------
{
  // OTA
  if (ota.begin()) {
    LOG.i("OTA startetd");
  } else {
    LOG.e("OTA failed");
  }
  LOG.i("OTA4");
}

// --------------------------------------------------------------------------------
void setupAutoconnectAndWebserver()
// --------------------------------------------------------------------------------
{
  // Autoconnect + Webserver

  //      Root
  webServer.on("/", []() { redirect(AC_ROOT); });

  //      Devicename
  webServer.on(AC_FACTORYRESET_SECTION_SET, []() {
    String sure = "false";
    webserverGetParameter(AC_FACTORYRESET_SECTION_SURE, sure);
    if (sure.equals("true")) {
      LOG.w("Perform Factory Reset", sure.c_str());
      reset.factoryReset();
    }
    redirect(AC_FACTORYRESET_SECTION);
  });

  //      Devicename
  webServer.on(AC_DEVICE_SECTION_SET, []() {
    String deviceName;
    if (webserverGetParameter(AC_DEVICE_SECTION_DEVICENAME, deviceName)) {
      autoconfigSet(AC_DEVICE_SECTION, AC_DEVICE_SECTION_DEVICENAME, deviceName);
      id = deviceName;
      setMDNSName(id);
      if (!nvs.writeString(NVS_DEVICENAME, deviceName, true)) {
        LOG.e("Could not write devicename to nvs");
      }
    }
    redirect(AC_DEVICE_SECTION);
  });

  //      Timezone
  webServer.on(AC_TIMEZONE_SECTION_SET, []() {
    String tz;
    if (webserverGetParameter(AC_TIMEZONE_SECTION_TIMEZONE, tz)) {
      autoconfigSet(AC_TIMEZONE_SECTION, AC_TIMEZONE_SECTION_TIMEZONE, tz);
      timezone = tz;
      if (state == STATE_HAS_NTP_TIME || state == STATE_HAS_TIMEZONE || state == STATE_NO_TIMEZONE) {
        state = STATE_HAS_NTP_TIME;
      }
      if (!nvs.writeString(NVS_TIMEZONE, timezone, true)) {
        LOG.e("Could not write timezone to nvs");
      }
    }
    redirect(AC_TIMEZONE_SECTION);
  });

  //        Load AC config
  AutoConnectConfig autoConnectConfig;
  autoConnectConfig.title = "ESP32Clock";
  autoConnectConfig.hostName = id;
  // autoConnectConfig.autoReconnect = true;
  autoConnectConfig.autoReconnect = false;
  if (autoConnect.load(configServerMenu)) {
    LOG.i("AutoConnect loaded.");
    if (autoConnect.begin()) {
      LOG.i("AutoConnect started.");
    } else {
      LOG.e(" Autoconnect start failed.");
    }
  } else {
    LOG.e("Autoconnect load failed.");
  }
  autoconfigSet(AC_DEVICE_SECTION, AC_DEVICE_SECTION_DEVICENAME, id);
  autoconfigSet(AC_TIMEZONE_SECTION, AC_TIMEZONE_SECTION_TIMEZONE, timezone);
}

// --------------------------------------------------------------------------------
void setupEzTime()
// --------------------------------------------------------------------------------
{
  // ezTime
  // setDebug(DEBUG);
  setInterval(60);
}

// --------------------------------------------------------------------------------
void setupStatistics()
// --------------------------------------------------------------------------------
{
  // Statistics
  if (statistics.begin()) {
    LOG.i("Statistics start");
  } else {
    LOG.e("Statistics start failed");
  }
}
/* #endregion */

/* #region  Main stuff */
// --------------------------------------------------------------------------------
void setup()
// --------------------------------------------------------------------------------
{
  state = STATE_BOOT;

  setupDisplay();
  setupSerial();

  showBootScreen();

  delay(1500);

  // Boot msg
  LOG.i("+-----------------------+");
  LOG.i("|        Booting        |");
  LOG.i("+-----------------------+");
  LOG.i("+ CPU frequency: %uMHz", esp.getCpuFreqMHz());
  LOG.i("+ Flash size: %u", esp.getFlashChipSize());
  LOG.i("+ SDK: %s", esp.getSdkVersion());
  LOG.i("+ CPU0 reset reason: %s -> %s ", reset.getResetReason0(), reset.getResetReasonVerbose0());
  LOG.i("+ CPU1 reset reason: %s -> %s ", reset.getResetReason1(), reset.getResetReasonVerbose1());
  LOG.i("+-----------------------+");

  setupNVS();
  setupDNS();
  setupWiFi();
  setupOTA();
  setupAutoconnectAndWebserver();
  setupEzTime();
  setupStatistics();

  state = STATE_BOOT_DONE;
}

uint64_t nextEvent = 0;
// --------------------------------------------------------------------------------
void loop()
// --------------------------------------------------------------------------------
{
  ota.loop();
  statistics.loop();
  if (!ota.isUpdating()) {
    switch (state) {
      case STATE_BOOT_DONE:
        if (lastNtpUpdateTime()) {
          state = STATE_HAS_NTP_TIME;
        }
        break;
      case STATE_HAS_NTP_TIME:
        // https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
        if (myTimezone.setLocation(timezone)) {
          LOG.i("Timezone set to ", myTimezone.getTimezoneName());
          state = STATE_HAS_TIMEZONE;
        } else {
          LOG.e("Timezone set failed, %s", errorString());
          state = STATE_NO_TIMEZONE;
        }
        break;
      case STATE_BOOT:
      case STATE_HAS_TIMEZONE:
      case STATE_NO_TIMEZONE:
        break;
    }

    events();

    autoConnect.handleClient();

    uint64_t now = millis();
    if (now > nextEvent) {
      nextEvent = now + 250;
      showTime();
    }
  }
}
/* #endregion */

/* #region  autocofig/webserver utils */
// --------------------------------------------------------------------------------
bool webserverGetParameter(const String& key, String& result)
// --------------------------------------------------------------------------------
{
  if (webServer.hasArg(key)) {
    result = webServer.arg(key);
    return true;
  } else {
    LOG.e("[ConfigServer] Error: arg '%s' not found in parameters", key.c_str());
  }
  return false;
}

// --------------------------------------------------------------------------------
bool autoconfigSet(const String& section, const String& name, const String& value)
// --------------------------------------------------------------------------------
{
  AutoConnectAux* device = autoConnect.aux(section);
  if (device) {
    AutoConnectInput& deviceNameInput = device->getElement<AutoConnectInput>(name);
    if (&deviceNameInput) {
      deviceNameInput.value = value;
      return true;
    } else {
      LOG.e("[ConfigServer] Error: could not found %s section in configuration", section.c_str());
    }
  } else {
    LOG.e("[ConfigServer] Error: could not found %s section in configuration", section.c_str());
  }
  return false;
}

// --------------------------------------------------------------------------------
void redirect(const String toLocation)
// --------------------------------------------------------------------------------
{
  webServer.sendHeader("Location", toLocation, true);
  webServer.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webServer.client().stop();
}
/* #endregion */

/* #region  dns stuff */
// --------------------------------------------------------------------------------
void setMDNSName(const String name)
// --------------------------------------------------------------------------------
{
  MDNS.end();
  if (MDNS.begin(name.c_str())) {
    LOG.i("mDNS start name: %s", id.c_str());
  } else {
    LOG.e("mDNS failed");
  }
}
/* #endregion */

/* #region  display stuff */
// --------------------------------------------------------------------------------
void showBootScreen()
// --------------------------------------------------------------------------------
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 20, "Booting...");
  u8g2.sendBuffer();
}

// --------------------------------------------------------------------------------
void showConnectScreen()
// --------------------------------------------------------------------------------
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 20, "Connect");
  u8g2.drawStr(0, 40, "to WiFi...");
  u8g2.sendBuffer();
}

// --------------------------------------------------------------------------------
void showConnectionFailed(uint8_t reason)
// --------------------------------------------------------------------------------
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 20, "WiFi failed");
  u8g2.drawStr(0, 40, "reason:");
  u8g2.drawStr(0, 60, getWifiFailReason(reason));
  u8g2.sendBuffer();
}

// --------------------------------------------------------------------------------
void showTime()
// --------------------------------------------------------------------------------
{
  char temp[3];
  uint8_t w;

  // hour + min
  char t[6];
  strcpy(temp, u8x8_u8toa(myTimezone.hour(), 2));
  t[0] = temp[0];
  t[1] = temp[1];
  t[2] = ':';
  strcpy(temp, u8x8_u8toa(myTimezone.minute(), 2));
  t[3] = temp[0];
  t[4] = temp[1];
  t[5] = 0;

  // second
  char sec[3];
  strcpy(sec, u8x8_u8toa(myTimezone.second(), 2));
  uint8_t s = (128 * ((float)myTimezone.second() + ((float)myTimezone.ms()) / 1000.0)) / 59;

  // date
  // works only from 2000...2099
  char d[11];
  strcpy(temp, u8x8_u8toa(myTimezone.day(), 2));
  d[0] = temp[0];
  d[1] = temp[1];
  d[2] = '.';
  strcpy(temp, u8x8_u8toa(myTimezone.month(), 2));
  d[3] = temp[0];
  d[4] = temp[1];
  d[5] = '.';
  d[6] = '2';
  d[7] = '0';
  strcpy(temp, u8x8_u8toa(myTimezone.year() - 2000, 2));
  d[8] = temp[0];
  d[9] = temp[1];
  d[10] = 0;

  // print
  u8g2.clearBuffer();
  u8g2.setDrawColor(1);

  // time
  u8g2.drawHLine(s - 5, 0, 10);
  u8g2.drawHLine(s - 5, 1, 10);
  u8g2.setFont(u8g2_font_freedoomr25_mn);
  w = u8g2.getStrWidth(t);
  u8g2.drawStr((128 - w) / 2, 32, t);

  // date
  u8g2.setFont(u8g2_font_t0_16_tn);
  w = u8g2.getStrWidth(d);
  u8g2.drawStr((128 - w) / 2, 46, d);

  // ip
  u8g2.setFont(u8g2_font_profont10_tf);
  w = u8g2.getStrWidth(currentIP.c_str());
  u8g2.drawStr(128 - w, 63, currentIP.c_str());

  u8g2.sendBuffer();
}
/* #endregion */
