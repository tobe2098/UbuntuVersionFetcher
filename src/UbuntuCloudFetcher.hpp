#pragma once

#include "UbuntuCloudInterface.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

class UbuntuCloudFetcher : public UbuntuCloudInterface {
    // For clarity

  private:
    //
    const std::string _url;          // The used URL for curl request
    bool              _initialized;  // Wether the instance has initialized json data
    json              _productData;  // Received data from curl request in json format

    bool curlRequest(CURL* curl_handle, std::string& curl_response);

  public:
    //
    UbuntuCloudFetcher(const std::string& url);

    ~UbuntuCloudFetcher();

    bool isInitialized() const override { return _initialized; }
    // Return the list of all currently *supported* Ubuntu releases
    std::vector<std::pair<std::string, std::vector<std::string>>> getSupportedReleases() const override;

    // Return the current Ubuntu LTS version
    std::string getCurrentLTS() const override;

    // Return the sha256 of the disk1.img of a given Ubuntu release (string match)
    std::optional<std::string> getSha256ForRelease(const std::string& release) const override;

    // Fetch data
    bool fetchData();
    // Process data
    // void processData(); //Not necessary because we only do one request per process

    static size_t writeData(void* buffer_ptr, size_t size, size_t nmemb, void* data_ptr);
};