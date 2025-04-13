/**
 * @file UbuntuCloudFetcher.cpp
 * @brief Implementation of the UbuntuCloudFetcher class for fetching and parsing Ubuntu cloud image metadata.
 *
 * This file provides the concrete implementation of the UbuntuCloudInterface interface, responsible for:
 * - Performing HTTP(S) requests to retrieve release metadata from the official Ubuntu cloud images server.
 * - Parsing the retrieved JSON data to extract release, architecture, and version details.
 * - Providing helper functions to query the latest LTS release, all supported releases, and SHA256 checksums for a release.
 *
 * Dependencies:
 * - libcurl: for HTTP requests.
 * - nlohmann::json: for JSON parsing.
 *
 * @note In this implementation the structure of the data is assumed from
 * https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json
 * @note This implementation prioritizes minimal memory overhead during version comparison and data extraction.
 */

#include "UbuntuCloudFetcher.hpp"

/**
 * @brief Compares two version strings to determine which one is more recent.
 *
 * @param first_version The reference version string.
 * @param second_version The version string to compare against the reference.
 * @return 1 if first_version is newer, -1 if second_version is newer, 0 if both are equal.
 */
static int isMoreRecentVersion(const std::string& first_version, const std::string& second_version) {
  // This method avoids using extra memory
  size_t min_len { std::min(first_version.size(), second_version.size()) };
  for (size_t idx = 0; idx < min_len; idx++) {
    if (first_version.at(idx) == second_version.at(idx)) {
      continue;  // equal
    } else if (first_version.at(idx) == '.') {
      return -1;  // first version is older
    } else if (second_version.at(idx) == '.') {
      return 1;  // first version is newer
    } else if (first_version.at(idx) > second_version.at(idx)) {
      return 1;  // first version is newer
    } else {
      return -1;  // first version is older
    }
  }
  return 0;  // both are equal
}

UbuntuCloudFetcher::UbuntuCloudFetcher(const std::string& url): _url(url), _initialized(false) {
  // Call fetch data to attemp to initialize the fetcher
  _initialized = fetchData();
}

UbuntuCloudFetcher::~UbuntuCloudFetcher() { }

std::vector<UbuntuRelease> UbuntuCloudFetcher::getSupportedReleases() const {
  std::map<std::string, std::vector<std::string>, std::greater<>> release_map;
  // Assume only unique version names are wanted in said list
  // The inherent sorting of the std::map will allow to separate the LTS from the non-LTS versions, for later handling

  for (const auto& [product_name, product_json] : this->_productData.items()) {
    // Iterate over each product entry
    if (product_json.find("supported") != product_json.end() && product_json.at("supported") == true) {
      // If the version is supported, we include its name in the map
      std::string complete_name =
        product_json.at("release_title").template get<std::string>() + " (" + product_json.at("release").template get<std::string>() + ")";
      release_map[complete_name].push_back(product_json.at("arch").template get<std::string>());
      // At the same time, we store the architectures of each release
    }
  }
  // The vector is constructed using the struct's constructor from std::pair<std::string,std::vector<std::string>>, which is the std::map's
  // underlying type
  return std::vector<UbuntuRelease>(release_map.begin(), release_map.end());
}

std::optional<UbuntuRelease> UbuntuCloudFetcher::getCurrentLTS() const {
  // The lts versions have LTS in release_title, the current version is the latest LTS
  std::optional<UbuntuRelease> current_LTS { std::nullopt };
  // We do not know if the current LTS is in the data, so optional is empty

  for (const auto& [product_name, product_json] : this->_productData.items()) {
    // Iterate over each product entry
    if (product_json.find("aliases") != product_json.end() &&
        product_json.at("aliases").template get<std::string>().find("lts") != std::string::npos) {
      // The lts flag in aliases is the latest/current LTS release
      if (!current_LTS) {
        // If optional is not initalized, initialize
        current_LTS = UbuntuRelease(
          { product_json.at("version").template get<std::string>() + " (" + product_json.at("release").template get<std::string>() + ")",
            {} });
        // Version instead of release_title because we do not need the LTS label
      }
      current_LTS->architectures.push_back(product_json.at("arch").template get<std::string>());
      // Add each architecture to the version
      std::string latest_version {};
      for (const auto& [version, version_contents] : product_json.at("versions").items()) {
        // Avoid assumption that entries are sorted
        if (latest_version.empty() || isMoreRecentVersion(latest_version, version) == -1) {
          latest_version = version;
          // Store latest version
        }
      }
      current_LTS->latest_versions.push_back(latest_version);
      // Store latest version in same order as architecture
    }
  }
  return current_LTS;
}

std::optional<std::string> UbuntuCloudFetcher::getSha256ForRelease(const std::string& release_name) const {
  // ID by title and by release_title or release. Both are shown in --supported-releases
  //  amd64 only in terms of releases, get the latest version in versions.
  std::optional<std::string> stringSha256 { std::nullopt };
  // We do not know if the version is in the data, so optional is empty

  auto is_release_lambda = [release_name](const json& product_json) -> bool {
    return (product_json.find("arch") != product_json.end() && product_json.at("arch").template get<std::string>().compare("amd64") == 0) &&
           ((product_json.find("release") != product_json.end() &&
             product_json.at("release").template get<std::string>().compare(release_name) == 0) ||
            (product_json.find("release_title") != product_json.end() &&
             product_json.at("release_title").template get<std::string>().compare(release_name) == 0));
    // The check against both the title and release in case user gives either, amd64 only
  };

  for (const auto& [product_name, product_json] : this->_productData.items()) {
    // Iterate over each product entry
    if (is_release_lambda(product_json)) {
      std::string latest_version {};
      for (const auto& [version, version_contents] : product_json.at("versions").items()) {
        // Avoid assumption that entries are sorted
        if (latest_version.empty() || isMoreRecentVersion(latest_version, version) == -1) {
          // Store the latest version and its sha256
          latest_version = version;
          stringSha256   = version_contents.at("items").at("disk1.img").at("sha256").template get<std::string>();
        }
      }
      return stringSha256;
    }
  }
  return std::nullopt;
}

bool UbuntuCloudFetcher::fetchData() {
  // Initialize the handle
  CURL*       curl_handle { nullptr };
  std::string curl_response {};

  curl_handle = curl_easy_init();
  if (!curl_handle) {
    // If the handle is not initialized properly, the request fails
    return false;  // Early return without clean-up required
  }
  // Separate this part with function for handle set-up? Not really possible bc of cleanup probably?
  //  Set curl options: url, writing function with set signature, data pointer for said function and timeout.
  curl_easy_setopt(curl_handle, CURLOPT_URL, this->_url.data());          // C-str only
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, this->writeData);  // Static member function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &(curl_response));     // Data pointer for the writeData static member function
  curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);                    // maximum time the transfer is allowed to complete
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);              // follow HTTP 3xx redirects

  bool result { curlRequest(curl_handle, curl_response) };  // Store success or failure for return after clean-up

  // Clean-up functions whatever result the request has
  curl_easy_cleanup(curl_handle);
  return result;  // Return result
}

bool UbuntuCloudFetcher::curlRequest(CURL* curl_handle, std::string& curl_response) {
  CURLcode result_code { curl_easy_perform(curl_handle) };
  // Curl code is CURLE_OK if everything went well.
  if (result_code != CURLE_OK) {
    // Print error with curl handler function
    std::cerr << "curl error: " << curl_easy_strerror(result_code) << std::endl;
    // Force flush to ensure output is clear
    return false;  // Early return
  }
  // After curl handle initialization, we do the request
  try {
    // Parse the received data
    json data = json::parse(curl_response);
    if (!data.contains("products")) {
      // Problem with data content, this key contains our data of interest
      std::cerr << "Data not found on curl request" << std::endl;
      return false;  // Early return
    }
    // Store data on success
    this->_productData = data.at("products");
  } catch (const json::parse_error& exception) {
    // Exception during parsing
    std::cerr << "JSON parsing error: " << exception.what() << std::endl;
    // Force flush to ensure output is clear
    return false;
  }
  return true;  // Return after success
}

size_t UbuntuCloudFetcher::writeData(void* buffer_ptr, size_t size, size_t nmemb, void* data_ptr) {
  std::string* string_ptr { static_cast<std::string*>(data_ptr) };
  // Append data to the string
  string_ptr->append(static_cast<char*>(buffer_ptr), size * nmemb);
  // Return processed data size in bytes
  return size * nmemb;
}
