#include "UbuntuCloudFetcher.hpp"

UbuntuCloudFetcher::UbuntuCloudFetcher(const std::string& url): _url(url), _initialized(false), _curl_handle(nullptr) {
  fetchData();
}

UbuntuCloudFetcher::~UbuntuCloudFetcher() { }

std::vector<std::string> UbuntuCloudFetcher::getSupportedReleases() const {
  // Check the "supported" boolean in the json
  return std::vector<std::string>();
}

std::string UbuntuCloudFetcher::getCurrentLTS() const {
  return std::string();
}

std::optional<std::string> UbuntuCloudFetcher::getSha256ForRelease(const std::string& release) const {
  return std::optional<std::string>();
}

bool UbuntuCloudFetcher::fetchData() {
  // Only initialize the curl easy handle if it has not been initialized successfully before
  if (!this->_curl_handle) {
    this->_curl_handle = curl_easy_init();
    if (!this->_curl_handle) {
      return false;
    }
    curl_easy_setopt(this->_curl_handle, CURLOPT_URL, this->_url);
    curl_easy_setopt(this->_curl_handle, CURLOPT_WRITEFUNCTION, this->writeData);
  }
  json data;
  return false;
}

size_t UbuntuCloudFetcher::writeData(void* buffer_ptr, size_t size, size_t nmemb, void* data_ptr) {
  return size_t();
}
