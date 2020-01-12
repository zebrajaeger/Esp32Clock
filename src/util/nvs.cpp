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

#include "util/nvs.h"

//------------------------------------------------------------------------------
NVS::NVS(const String& name)
    : LOG("NVS[" + name + "]"),
      name_(name),
      handle_(0)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool NVS::begin()
//------------------------------------------------------------------------------
{
  if (handle_) {
    LOG.e("Already opened");
    return 0;
  }

  esp_err_t err;

  // initialize;
  err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    LOG.i("Storage needs to be erased");
    err = nvs_flash_erase();
    if (err == ESP_OK) {
      LOG.i("Storage erased");
    } else {
      LOG.e("Storage not erased. Reason: %s", esp_err_to_name(err));
    }
    err = nvs_flash_init();
  }

  if (err == ESP_OK) {
    LOG.i("Storage initialized");

    // open
    err = nvs_open(name_.c_str(), NVS_READWRITE, &handle_);
    if (err == ESP_OK) {
      LOG.i("Storage opened");
      return true;
    } else {
      LOG.e("Storage not opened. Reason: %s", esp_err_to_name(err));
    }
  } else {
    LOG.e("Storage not initializued. Reason: %s", esp_err_to_name(err));
  }
  return false;
}

//------------------------------------------------------------------------------
void NVS::end()
//------------------------------------------------------------------------------
{
  if (handle_) {
    nvs_close(handle_);
  }
  handle_ = 0;
}

//------------------------------------------------------------------------------
esp_err_t NVS::existsString(const String& key)
//------------------------------------------------------------------------------
{
  if (!handle_) {
    return -1;
  }

  size_t l;
  return nvs_get_str(handle_, key.c_str(), NULL, &l);
}

//------------------------------------------------------------------------------
bool NVS::readString(const String& key, String& value)
//------------------------------------------------------------------------------
{
  if (!handle_) {
    return false;
  }

  esp_err_t err;
  size_t l;

  // read size
  err = nvs_get_str(handle_, key.c_str(), NULL, &l);
  if (err == ESP_OK) {
    // read value
    l++;  // last zero byte
    char buffer[l];
    err = nvs_get_str(handle_, key.c_str(), buffer, &l);
    value = (char*)&buffer;
    if (err == ESP_OK) {
      return true;
    } else {
      LOG.e("Value not read. Reason: %s", esp_err_to_name(err));
    }
  } else {
    LOG.e("Value size not read. Reason: %s", esp_err_to_name(err));
  }
  return false;
}

//------------------------------------------------------------------------------
bool NVS::writeString(const String& key, const String& value, bool commitAfterWrite)
//------------------------------------------------------------------------------
{
  esp_err_t err;
  err = nvs_set_str(handle_, key.c_str(), value.c_str());
  if (err == ESP_OK) {
    if (commitAfterWrite) {
      return commit();
    } else {
      return true;
    }
  } else {
    LOG.e("Value not written. Reason: %s", esp_err_to_name(err));
  }
  return false;
}

//------------------------------------------------------------------------------
bool NVS::commit()
//------------------------------------------------------------------------------
{
  esp_err_t err;
  err = nvs_commit(handle_);
  if (err == ESP_OK) {
    return true;
  } else {
    LOG.e("Not commited. Reason: %s", esp_err_to_name(err));
  }
  return false;
}
