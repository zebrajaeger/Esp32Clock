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


#include "util/reset.h"

#include <WiFi.h>
//------------------------------------------------------------------------------
Reset::Reset()
    : LOG("RESET")
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
void Reset::factoryReset()
//------------------------------------------------------------------------------
{
  LOG.i("Erase WIFI data");
  // hack due to ESP-HAL Bug
  WiFi.disconnect(true);  // still not erasing the ssid/pw. Will happily reconnect on next start
  WiFi.begin("0", "0");   // adding this effectively seems to erase the previous stored SSID/PW

  LOG.i("Erase NVS data");
  const esp_partition_t* part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
  if (part) {
    LOG.i("found partition '%s' at offset 0x%x with size 0x%x", part->label, part->address, part->size);
    esp_err_t err = esp_partition_erase_range(part, 0, part->size);
    if (err) {
      LOG.e("Could not erase NVS partition. Reason: %s", esp_err_to_name(err));
    } else {
      LOG.i("partition erased.");
    }
  }

  ESP.restart();
  delay(1000);
}

//------------------------------------------------------------------------------
const char* Reset::getResetReason(RESET_REASON reason)
//------------------------------------------------------------------------------
{
  switch (reason) {
    case 1:
      return "POWERON_RESET";
      break; /**<1,  Vbat power on reset*/
    case 3:
      return "SW_RESET";
      break; /**<3,  Software reset digital core*/
    case 4:
      return "OWDT_RESET";
      break; /**<4,  Legacy watch dog reset digital core*/
    case 5:
      return "DEEPSLEEP_RESET";
      break; /**<5,  Deep Sleep reset digital core*/
    case 6:
      return "SDIO_RESET";
      break; /**<6,  Reset by SLC module, reset digital core*/
    case 7:
      return "TG0WDT_SYS_RESET";
      break; /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8:
      return "TG1WDT_SYS_RESET";
      break; /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9:
      return "RTCWDT_SYS_RESET";
      break; /**<9,  RTC Watch dog Reset digital core*/
    case 10:
      return "INTRUSION_RESET";
      break; /**<10, Instrusion tested to reset CPU*/
    case 11:
      return "TGWDT_CPU_RESET";
      break; /**<11, Time Group reset CPU*/
    case 12:
      return "SW_CPU_RESET";
      break; /**<12, Software reset CPU*/
    case 13:
      return "RTCWDT_CPU_RESET";
      break; /**<13, RTC Watch dog Reset CPU*/
    case 14:
      return "EXT_CPU_RESET";
      break; /**<14, for APP CPU, reseted by PRO CPU*/
    case 15:
      return "RTCWDT_BROWN_OUT_RESET";
      break; /**<15, Reset when the vdd voltage is not stable*/
    case 16:
      return "RTCWDT_RTC_RESET";
      break; /**<16, RTC Watch dog reset digital core and rtc module*/
    default:
      return "NO_MEAN";
  }
}

//------------------------------------------------------------------------------
const char* Reset::getVerboseResetReason(RESET_REASON reason)
//------------------------------------------------------------------------------
{
  switch (reason) {
    case 1:
      return "Vbat power on reset";
      break;
    case 3:
      return "Software reset digital core";
      break;
    case 4:
      return "Legacy watch dog reset digital core";
      break;
    case 5:
      return "Deep Sleep reset digital core";
      break;
    case 6:
      return "Reset by SLC module, reset digital core";
      break;
    case 7:
      return "Timer Group0 Watch dog reset digital core";
      break;
    case 8:
      return "Timer Group1 Watch dog reset digital core";
      break;
    case 9:
      return "RTC Watch dog Reset digital core";
      break;
    case 10:
      return "Instrusion tested to reset CPU";
      break;
    case 11:
      return "Time Group reset CPU";
      break;
    case 12:
      return "Software reset CPU";
      break;
    case 13:
      return "RTC Watch dog Reset CPU";
      break;
    case 14:
      return "for APP CPU, reseted by PRO CPU";
      break;
    case 15:
      return "Reset when the vdd voltage is not stable";
      break;
    case 16:
      return "RTC Watch dog reset digital core and rtc module";
      break;
    default:
      return "NO_MEAN";
  }
}