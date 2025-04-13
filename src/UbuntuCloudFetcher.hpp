/**
 * @file UbuntuCloudFetcher.hpp
 * @brief Provides an implementation of UbuntuCloudInterface using remote JSON data fetched via libcurl.
 *
 * This file defines the UbuntuCloudFetcher class, which retrieves, parses, and processes Ubuntu
 * cloud image release information from a specified URL. The class offers methods for querying
 * supported releases, the current Long Term Support (LTS) release, and SHA256 checksums for specific releases.
 *
 * The implementation uses libcurl for HTTP requests and the nlohmann::json library for JSON parsing.
 *
 * Intended for use in applications that need to automate or display Ubuntu cloud image information
 * fetched from an online source.
 */

#pragma once
#include "UbuntuCloudInterface.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

/**
 * @brief Implementation of UbuntuCloudInterface that fetches Ubuntu release data from a cloud source.
 *
 * This class fetches and processes Ubuntu cloud images information from a provided URL,
 * allowing queries for supported releases, LTS versions, and SHA256 hashes.
 */
class UbuntuCloudFetcher : public UbuntuCloudInterface {
  private:
    /// The URL used for curl request.
    const std::string _url;
    /// Whether the instance has successfully initialized json data.
    bool _initialized;
    /// Received data from curl request in json format.
    json _productData;

    /**
     * @brief Performs the actual HTTP request using libcurl and parses the server's JSON response.
     * @param curl_handle The curl handle to use.
     * @param curl_response String reference to store the server's raw response.
     * @return bool True if the request was successful and the data was valid; false otherwise.
     * @note Result stored in class member _productData.
     */
    bool curlRequest(CURL* curl_handle, std::string& curl_response);

  public:
    /**
     * @brief Constructor that sets the URL for fetching data.
     *
     * The constructor calls fetchData(), and sets the class member _initizalied to the return value of the call.
     * @param url The URL to fetch data from.
     */
    UbuntuCloudFetcher(const std::string& url);

    ~UbuntuCloudFetcher();

    /**
     * @brief Checks if the fetcher has successfully initialized.
     * @return bool True if initialized (_productData contains json with parsed data), false otherwise.
     */
    bool isInitialized() const override { return _initialized; }

    /**
     * @brief Returns a list of all currently supported Ubuntu releases.
     * @return std::vector<UbuntuRelease> List of supported releases.
     * @note Requires the fetcher to be properly initialized.
     */
    std::vector<UbuntuRelease> getSupportedReleases() const override;

    /**
     * @brief Retrieves the current Long Term Support (LTS) Ubuntu release, if available.
     * @return An optional UbuntuRelease object containing the current LTS release information, or std::nullopt if none found.
     */
    std::optional<UbuntuRelease> getCurrentLTS() const override;

    /**
     * @brief Retrieves the SHA256 checksum for a given Ubuntu release for amd64, if available.
     *
     * The function searches for releases matching the provided name and architecture "amd64".
     * It returns the checksum of the latest version found.
     *
     * @param release_name The name or title of the release to search for, either in numeric format (22.04) or by release format (noble).
     * @return An optional string containing the SHA256 checksum, or std::nullopt if no match is found.
     */
    std::optional<std::string> getSha256ForRelease(const std::string& release) const override;

    /**
     * @brief Fetches Ubuntu product data from the configured URL using libcurl.
     * @return bool True if the data was successfully fetched and parsed; false otherwise.
     * @note Should be called after construction to initialize the fetcher.
     */
    [[nodiscard]] bool fetchData();

    /**
     * @brief Static callback function for curl to write received data.
     * @param[in] buffer_ptr Pointer to the received data.
     * @param[in] size Size of each data element.
     * @param[in] nmemb Number of data elements.
     * @param[out] data_ptr Pointer to the data storage location.
     * @return size_t Number of bytes processed.
     */
    static size_t writeData(void* buffer_ptr, size_t size, size_t nmemb, void* data_ptr);
    // Method has to be static because libcurl is a C library and does not support C++ member functions directly. If multi-threaded support
    // is desired, use the static method to call a non-static method that does the actual data writing.
};