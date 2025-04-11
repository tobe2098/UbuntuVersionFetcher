#include "UbuntuCloudFetcher.hpp"

// static std::vector<int> partitionVersionString(const std::string& version) {
//   //With this method we avoid creating a string copy
//   std::vector<int> parts;

//   int num {}, version_size { static_cast<int>(version.size()) };
//   for (int idx = 0; idx < version_size; idx++) {
//     if (version.at(idx) == '.') {
//       parts.push_back(num);
//       num = 0;
//     } else {
//       num *= 10;
//       num += version.at(idx) - '0';
//     }
//   }
//   return parts;
// }

static int isMoreRecentVersion(const std::string& old_version, const std::string& contender_version) {
  // std::vector<int> old_parts { partitionVersionString(old_version) };
  // std::vector<int> new_parts { partitionVersionString(contender_version) };
  // This method avoids using extra memory
  size_t min_len { std::min(old_version.size(), contender_version.size()) };
  for (size_t idx = 0; idx < min_len; idx++) {
    if (old_version.at(idx) == contender_version.at(idx)) {
      continue;  // equal
    } else if (old_version.at(idx) == '.') {
      return -1;  // old version is smaller
    } else if (contender_version.at(idx) == '.') {
      return 1;  // old version is bigger
    } else if (old_version.at(idx) > contender_version.at(idx)) {
      return 1;  // old version is bigger
    } else {
      return -1;  // old version is smaller
    }
  }
  return 0;  // equal
}

UbuntuCloudFetcher::UbuntuCloudFetcher(const std::string& url): _url(url), _initialized(false) {
  fetchData();
}

UbuntuCloudFetcher::~UbuntuCloudFetcher() { }

std::vector<UbuntuRelease> UbuntuCloudFetcher::getSupportedReleases() const {
  std::map<std::string, std::vector<std::string>, std::greater<>> unique_releases;
  // Assume only unique version names are wanted in said list
  // The sorting will allow to separate the LTS from the non-LTS versions, for later handling

  for (const auto& [product_name, product_json] : this->_productData.items()) {
    // Iterate over each product entry
    if (product_json.find("supported") != product_json.end() && product_json.at("supported") == true) {
      // If the version is supported, we include its name in the set
      std::string complete_name =
        product_json.at("release_title").template get<std::string>() + " (" + product_json.at("release").template get<std::string>() + ")";
      unique_releases[complete_name].push_back(product_json.at("arch").template get<std::string>());
    }
  }
  // Maybe sort versions?
  //  Check the "supported" boolean in the json
  return std::vector<UbuntuRelease>(unique_releases.begin(), unique_releases.end());
}

std::optional<UbuntuRelease> UbuntuCloudFetcher::getCurrentLTS() const {
  // The lts versions have LTS in release_title, the current version is the latest LTS
  std::optional<UbuntuRelease> current_LTS;
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
      std::string latest_version;
      for (const auto& [version, version_contents] : product_json.at("versions").items()) {
        // Avoid assumption that entries are sorted
        if (latest_version.empty() || isMoreRecentVersion(latest_version, version) == -1) {
          latest_version = version;
        }
      }
      current_LTS->latest_versions.push_back(latest_version);
    }
  }
  return current_LTS;
}

std::optional<std::string> UbuntuCloudFetcher::getSha256ForRelease(const std::string& release_name) const {
  // ID by title and by release_title or release
  //  amd64 only in terms of releases, get the latest version in versions.
  std::optional<std::string> stringSha256;
  // We do not know if the version is in the data, so optional is empty

  auto is_release_lambda = [release_name](const json& product_json) -> bool {
    return (product_json.find("arch") != product_json.end() &&
            product_json.at("arch").template get<std::string>().find("amd64") != std::string::npos) &&
           ((product_json.find("release") != product_json.end() &&
             product_json.at("release").template get<std::string>().find(release_name) != std::string::npos) ||
            (product_json.find("release_title") != product_json.end() &&
             product_json.at("release_title").template get<std::string>().find(release_name) != std::string::npos));
    // The check against both the title and release in case user gives either, amd64 only
  };

  for (const auto& [product_name, product_json] : this->_productData.items()) {
    // Iterate over each product entry
    if (is_release_lambda(product_json)) {
      std::string latest_version;
      for (const auto& [version, version_contents] : product_json.at("versions").items()) {
        // Avoid assumption that entries are sorted
        if (latest_version.empty() || isMoreRecentVersion(latest_version, version) == -1) {
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
