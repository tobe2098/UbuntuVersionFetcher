#include "UbuntuCloudFetcher.hpp"

UbuntuCloudFetcher::UbuntuCloudFetcher(const std::string& url) { }

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
  return false;
}
