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
#include <esp_partition.h>
#include <rom/rtc.h>

#include "util/logger.h"

// from https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ResetReason/ResetReason.ino
class Reset {
 public:
  Reset();

  const char* getResetReason0() { return getResetReason(rtc_get_reset_reason(0)); }
  const char* getResetReasonVerbose0() { return getVerboseResetReason(rtc_get_reset_reason(0)); }
  const char* getResetReason1() { return getResetReason(rtc_get_reset_reason(1)); }
  const char* getResetReasonVerbose1() { return getVerboseResetReason(rtc_get_reset_reason(1)); }

  void factoryReset();

 private:
  const char* getResetReason(RESET_REASON reason);
  const char* getVerboseResetReason(RESET_REASON reason);
  Logger LOG;
};