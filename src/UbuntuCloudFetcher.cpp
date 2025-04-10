#include "UbuntuCloudFetcher.hpp"

UbuntuCloudFetcher::UbuntuCloudFetcher(const std::string& url): _url(url), _initialized(false) {
  fetchData();
}

UbuntuCloudFetcher::~UbuntuCloudFetcher() { }

std::vector<std::string> UbuntuCloudFetcher::getSupportedReleases() const {
  std::set<std::string> unique_versions;  // Assume only unique version names are wanted in said list

  for (const auto& [product_name, product_values] : this->_productData.items()) {
    // Iterate over each product entry
    if (product_values.find("supported") != product_values.end() && product_values.at("supported") == "true") {
      // If the version is supported, we include its name in the set
      std::string complete_name = "Ubuntu" + product_values.at("release_title").template get<std::string>() + " (" +
                                  product_values.at("release_name").template get<std::string>() +
                                  ") : " + product_values.at("arch").template get<std::string>();
      unique_versions.insert(complete_name);
      // unique_versions.insert(product_values["release_title"]);
    }
  }
  // Maybe sort versions?
  //  Check the "supported" boolean in the json
  return std::vector<std::string>(unique_versions.begin(), unique_versions.end());
}

std::string UbuntuCloudFetcher::getCurrentLTS() const {
  // The lts versions have LTS in release_title, the current version is the latest LTS
  return std::string();
}

std::optional<std::string> UbuntuCloudFetcher::getSha256ForRelease(const std::string& release) const {
  // amd64 only in terms of releases, get the latest version.
  return std::optional<std::string>();
}

bool UbuntuCloudFetcher::fetchData() {
  // Initialize the handle
  CURL*       curl_handle;
  std::string curl_response {};

  curl_handle = curl_easy_init();
  if (!curl_handle) {
    // If the handle is not initialized properly, the request fails
    return false;  // Early return without clean-up required
  }
  // Set curl options: url, writing function with set signature, data pointer for said function and timeout.
  curl_easy_setopt(curl_handle, CURLOPT_URL, this->_url.data());
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, this->writeData);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &(curl_response));
  curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

  bool result { curlRequest(curl_handle, curl_response) };

  // Clean-up functions whatever result the request has
  curl_easy_cleanup(curl_handle);
  return result;  // Return result
}

bool UbuntuCloudFetcher::curlRequest(CURL* curl_handle, std::string& curl_response) {
  CURLcode result_code = curl_easy_perform(curl_handle);
  // std::cout << curl_response;
  if (result_code != CURLE_OK) {
    // Curl request failed, clean-up functions
    std::cerr << "curl error: " << curl_easy_strerror(result_code) << std::endl;
    // Force flush to ensure output is clear
    return false;  // Early return
  }
  // After curl handle initialization, we do the request
  try {
    // json data = json::parse(
    //   "{\"content_id\": \"com.ubuntu.cloud:released:download\",\"creator\": \"climdb-download-streams-export\",\"datatype\": "
    //   "\"image-downloads\",\"format\": \"products:1.0\"\n}");
    json data = json::parse(curl_response);
    if (!data.contains("products")) {
      // Problem with data content
      std::cerr << "Data not found on curl request" << std::endl;
      return false;  // Early return
    }
    this->_productData = data["products"];
    this->_initialized = true;
  } catch (const json::parse_error& exception) {
    std::cerr << "JSON parsing error" << exception.what() << std::endl;
    // Force flush to ensure output is clear
    return false;
  }
  return true;  // Return after success
}

size_t UbuntuCloudFetcher::writeData(void* buffer_ptr, size_t size, size_t nmemb, void* data_ptr) {
  std::string* string_ptr { static_cast<std::string*>(data_ptr) };
  string_ptr->append((char*)buffer_ptr, size * nmemb);
  return size * nmemb;
}
