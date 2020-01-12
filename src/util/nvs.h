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
#include <nvs.h>
#include <nvs_flash.h>

#include "util/logger.h"

class NVS {
 public:
  NVS(const String& name);
  
  bool begin();
  void end();

  esp_err_t existsString(const String& key);
  bool readString(const String& key, String& value);
  bool writeString(const String& key, const String& value, bool commitAfterWrite = false);
  
  bool commit();

 private:
  Logger LOG;
  String name_;
  nvs_handle handle_;
};