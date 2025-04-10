#include "UbuntuCloudFetcher.hpp"

UbuntuCloudFetcher::UbuntuCloudFetcher(const std::string& url): _url(url), _initialized(false) {
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
  // Initialize the handle
  CURL*       curl_handle;
  std::string response;

  curl_handle = curl_easy_init();
  if (!curl_handle) {
    // If the handle is not initialized properly, the request fails
    return false;  // Early return
  }
  // Set curl options: url, writing function with set signature, data pointer for said function.
  curl_easy_setopt(curl_handle, CURLOPT_URL, this->_url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, this->writeData);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);

  CURLcode result_code = curl_easy_perform(curl_handle);

  if (result_code != CURLE_OK) {
    // Curl request failed, clean-up functions
    curl_easy_cleanup(curl_handle);
    return false;  // Early return
  }

  // Clean-up functions for curl request
  curl_easy_cleanup(curl_handle);
  return true;  // Return after success
}

size_t UbuntuCloudFetcher::writeData(void* buffer_ptr, size_t size, size_t nmemb, void* data_ptr) {
  std::string* string_ptr { static_cast<std::string*>(data_ptr) };
  string_ptr->assign((char*)buffer_ptr, size * nmemb);
  return string_ptr->size();
}
