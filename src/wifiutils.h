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

#pragma once

#include <Arduino.h>
#include <WiFi.h>

// --------------------------------------------------------------------------------
const char* getWifiEventName(WiFiEvent_t e)
// --------------------------------------------------------------------------------
{
  switch (e) {
    case SYSTEM_EVENT_WIFI_READY:
      return "SYSTEM_EVENT_WIFI_READY";
      break;
    case SYSTEM_EVENT_SCAN_DONE:
      return "SYSTEM_EVENT_SCAN_DONE";
      break;
    case SYSTEM_EVENT_STA_START:
      return "SYSTEM_EVENT_STA_START";
      break;
    case SYSTEM_EVENT_STA_STOP:
      return "SYSTEM_EVENT_STA_STOP";
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      return "SYSTEM_EVENT_STA_CONNECTED";
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      return "SYSTEM_EVENT_STA_DISCONNECTED";
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      return "SYSTEM_EVENT_STA_AUTHMODE_CHANGE";
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      return "SYSTEM_EVENT_STA_GOT_IP";
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      return "SYSTEM_EVENT_STA_LOST_IP";
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      return "SYSTEM_EVENT_STA_WPS_ER_SUCCESS";
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      return "SYSTEM_EVENT_STA_WPS_ER_FAILED";
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      return "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT";
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      return "SYSTEM_EVENT_STA_WPS_ER_PIN";
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP:
      return "SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP";
      break;
    case SYSTEM_EVENT_AP_START:
      return "SYSTEM_EVENT_AP_START";
      break;
    case SYSTEM_EVENT_AP_STOP:
      return "SYSTEM_EVENT_AP_STOP";
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      return "SYSTEM_EVENT_AP_STACONNECTED";
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      return "SYSTEM_EVENT_AP_STADISCONNECTED";
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      return "SYSTEM_EVENT_AP_STAIPASSIGNED";
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      return "SYSTEM_EVENT_AP_PROBEREQRECVED";
      break;
    case SYSTEM_EVENT_GOT_IP6:
      return "SYSTEM_EVENT_GOT_IP6";
      break;
    case SYSTEM_EVENT_ETH_START:
      return "SYSTEM_EVENT_ETH_START";
      break;
    case SYSTEM_EVENT_ETH_STOP:
      return "SYSTEM_EVENT_ETH_STOP";
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      return "SYSTEM_EVENT_ETH_CONNECTED";
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      return "SYSTEM_EVENT_ETH_DISCONNECTED";
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      return "SYSTEM_EVENT_ETH_GOT_IP";
      break;
    case SYSTEM_EVENT_MAX:
      return "SYSTEM_EVENT_MAX";
      break;
    default:
      return "UNKNOWN";
  }
}
// --------------------------------------------------------------------------------
const char* getWifiFailReason(uint8_t r)
// --------------------------------------------------------------------------------
{
  switch (r) {
    case 1:
      return "UNSPECIFIED";
      break;
    case 2:
      return "AUTH_EXPIRE";
      break;
    case 3:
      return "AUTH_LEAVE";
      break;
    case 4:
      return "ASSOC_EXPIRE";
      break;
    case 5:
      return "ASSOC_TOOMANY";
      break;
    case 6:
      return "NOT_AUTHED";
      break;
    case 7:
      return "NOT_ASSOCED";
      break;
    case 8:
      return "ASSOC_LEAVE";
      break;
    case 9:
      return "ASSOC_NOT_AUTHED";
      break;
    case 10:
      return "DISASSOC_PWRCAP_BAD";
      break;
    case 11:
      return "DISASSOC_SUPCHAN_BAD";
      break;
    case 13:
      return "IE_INVALID";
      break;
    case 14:
      return "MIC_FAILURE";
      break;
    case 15:
      return "4WAY_HANDSHAKE_TIMEOUT";
      break;
    case 16:
      return "GROUP_KEY_UPDATE_TIMEOUT";
      break;
    case 17:
      return "IE_IN_4WAY_DIFFERS";
      break;
    case 18:
      return "GROUP_CIPHER_INVALID";
      break;
    case 19:
      return "PAIRWISE_CIPHER_INVALID";
      break;
    case 20:
      return "AKMP_INVALID";
      break;
    case 21:
      return "UNSUPP_RSN_IE_VERSION";
      break;
    case 22:
      return "INVALID_RSN_IE_CAP";
      break;
    case 23:
      return "802_1X_AUTH_FAILED";
      break;
    case 24:
      return "CIPHER_SUITE_REJECTED";
      break;
    case 200:
      return "BEACON_TIMEOUT";
      break;
    case 201:
      return "NO_AP_FOUND";
      break;
    case 202:
      return "AUTH_FAIL";
      break;
    case 203:
      return "ASSOC_FAIL";
      break;
    case 204:
      return "HANDSHAKE_TIMEOUT";
      break;
    default:
      return "UNKOWN";
  }
}