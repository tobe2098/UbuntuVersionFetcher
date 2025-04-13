/**
 * @file UbuntuCloudFactory.hpp
 * @brief Declares the UbuntuCloudFactory class, which provides factory methods for creating UbuntuCloudInterface instances.
 *
 * This header defines a factory class responsible for encapsulating the creation logic of
 * UbuntuCloudInterface implementations. This pattern allows client code to remain decoupled
 * from the specific class used to fetch Ubuntu release metadata.
 */

#pragma once

#include "UbuntuCloudFetcher.hpp"
/**
 * @brief Factory class for creating UbuntuCloudInterface instances
 *
 * This factory encapsulates the creation logic for UbuntuCloudInterface implementations,
 * abstracting the specific implementation details from client code.
 */
class UbuntuCloudFactory {
    // Factory class for our UbuntuCloudInterface to abstract and encapsulate the fetcher's interface

  public:
    /**
     * @brief Creates a cloud interface fetcher for Ubuntu release information
     *
     * Factory function that creates and returns a new UbuntuCloudInterface implementation
     * configured to fetch data from the official Ubuntu cloud images stream.
     *
     * @return std::unique_ptr<UbuntuCloudInterface> A smart pointer to a newly created
     *         UbuntuCloudFetcher instance that implements the UbuntuCloudInterface
     *
     * @note The returned pointer is owned by the caller, who is responsible for
     *       its lifetime management
     */
    static std::unique_ptr<UbuntuCloudInterface> createUbuntuVersionFetcher() {
      std::string url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json";
      return std::make_unique<UbuntuCloudFetcher>(url);
    }
};
