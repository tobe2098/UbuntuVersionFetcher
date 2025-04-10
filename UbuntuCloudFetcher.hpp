#pragma once

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

class UbuntuCloudInterface {
  public:
    virtual ~UbuntuCloudInterface() = default;

    // Return a list of all currently supported Ubuntu releases
    virtual std::vector<std::string> getSupportedReleases() const = 0;

    // Return the current Ubuntu LTS version
    virtual std::string getCurrentLTS() const = 0;

    // Return the sha256 of the disk1.img item of a given Ubuntu release
    virtual std::optional<std::string> getSha256ForRelease(const std::string& release) const = 0;

    // Refresh data from the source
    virtual bool fetchData() = 0;
};

class UbuntuCloudFetcher : public UbuntuCloudInterface {
    const std::string _url;
    bool              _initialized;
    json              _productData;
    CURL*             _curl_handle;

  public:
    // Changing the url (only if input with --url?)
    UbuntuCloudFetcher(
      const std::string& url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json");

    ~UbuntuCloudFetcher();

    // Return the list of all currently *supported* Ubuntu releases
    std::vector<std::string> getSupportedReleases() const override;

    // Return the current Ubuntu LTS version
    std::string getCurrentLTS() const override;

    // Return the sha256 of the disk1.img of a given Ubuntu release (string match)
    std::optional<std::string> getSha256ForRelease(const std::string& release) const override;

    // Fetch data
    bool fetchData() override;

    static size_t writeData(void* buffer_ptr, size_t size, size_t nmemb, void* data_ptr);
};