// UbuntuCloudFactory.h
#pragma once

#include "UbuntuCloudFetcher.hpp"

class UbuntuCloudFactory {
  public:
    static std::unique_ptr<UbuntuCloudInterface> createFetcher() {
      // std::string url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud%3Areleased%3Adownload.json";
      std::string url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json";
      return std::make_unique<UbuntuCloudFetcher>(url);
    }
};
