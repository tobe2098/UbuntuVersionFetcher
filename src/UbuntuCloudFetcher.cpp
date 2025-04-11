#include "UbuntuCloudFetcher.hpp"

UbuntuCloudFetcher::UbuntuCloudFetcher(const std::string& url): _url(url), _initialized(false) {
  fetchData();
}

UbuntuCloudFetcher::~UbuntuCloudFetcher() { }

std::vector<UbuntuRelease> UbuntuCloudFetcher::getSupportedReleases() const {
  std::map<std::string, std::vector<std::string>, std::greater<>> unique_versions;
  // Assume only unique version names are wanted in said list

  for (const auto& [product_name, product_json] : this->_productData.items()) {
    // Iterate over each product entry
    if (product_json.find("supported") != product_json.end() && product_json.at("supported") == true) {
      // If the version is supported, we include its name in the set
      std::string complete_name =
        product_json.at("release_title").template get<std::string>() + " (" + product_json.at("release").template get<std::string>() + ")";
      unique_versions[complete_name].push_back(product_json.at("arch").template get<std::string>());
    }
    // unique_versions.insert(product_json["release_title"]);
  }
  // Maybe sort versions?
  //  Check the "supported" boolean in the json
  return std::vector<UbuntuRelease>(unique_versions.begin(), unique_versions.end());
}

std::optional<UbuntuRelease> UbuntuCloudFetcher::getCurrentLTS() const {
  // The lts versions have LTS in release_title, the current version is the latest LTS
  std::optional<UbuntuRelease> current_LTS;
  // We do not know if the current LTS is in the data, so optional is empty

  for (const auto& [product_name, product_json] : this->_productData.items()) {
    // Iterate over each product entry
    if (product_json.find("aliases") != product_json.end() &&
        product_json.at("aliases").template get<std::string>().find("lts") != std::string::npos) {
      // If the version is supported, we include its name in the set
      if (!current_LTS) {
        // If optional is not initalized, initialize
        current_LTS = UbuntuRelease({ product_json.at("release_title").template get<std::string>() + " (" +
                                        product_json.at("release").template get<std::string>() + ")",
                                      {} });
      }
      current_LTS->architectures.push_back(product_json.at("arch").template get<std::string>());
    }
  }
  return current_LTS;
}

std::optional<std::string> UbuntuCloudFetcher::getSha256ForRelease(const std::string& release) const {
  // ID by title and by relasename
  //  amd64 only in terms of releases, get the latest version in versions.
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
    json data = json::parse(curl_response);
    if (!data.contains("products")) {
      // Problem with data content
      std::cerr << "Data not found on curl request" << std::endl;
      return false;  // Early return
    }
    this->_productData = data.at("products");
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

UbuntuRelease::UbuntuRelease(): release_name(), architectures() { }

UbuntuRelease::UbuntuRelease(const std::pair<std::string, std::vector<std::string>>& data):
    release_name(data.first), architectures(data.second) { }
