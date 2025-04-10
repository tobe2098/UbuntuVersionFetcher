// UbuntuCloudFactory.h
#pragma once

#include "UbuntuCloudFetcher.hpp"

class UbuntuCloudFactory {
  public:
    static std::unique_ptr<UbuntuCloudInterface> createFetcher(
      const std::string& url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json") {
      return std::make_unique<UbuntuCloudFetcher>(url);
    }
};
