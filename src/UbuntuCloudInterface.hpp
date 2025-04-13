
/**
 * @file UbuntuCloudInterface.hpp
 * @brief Declares the UbuntuCloudInterface abstract class and the UbuntuRelease data structure for handling Ubuntu cloud image metadata.
 *
 * This header defines the core data model and interface for working with Ubuntu cloud releases.
 * The `UbuntuRelease` struct models metadata for each release, including its name, supported architectures,
 * and known version identifiers. The `UbuntuCloudInterface` abstract class defines the contract for
 * any implementation that fetches or provides Ubuntu release information from various sources (e.g. web APIs, local caches).
 *
 * Classes implementing this interface are expected to support release enumeration, LTS detection,
 * checksum retrieval for cloud images, and a basic initialization check.
 */

#pragma once

#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;  ///< Alias to reduce verbosity when using nlohmann::json.

/**
 * @struct UbuntuRelease
 * @brief Represents metadata for an Ubuntu release, including its name, supported architectures, and available versions.
 */
struct UbuntuRelease {
    std::string              release_name;     ///< The human-readable name of the release.
    std::vector<std::string> architectures;    ///< List of supported architectures for this release.
    std::vector<std::string> latest_versions;  ///< List of the latest known versions for this release corresponding to the architectures.
    /**
     * @brief Default constructor. Initializes an empty UbuntuRelease.
     */
    UbuntuRelease(): release_name(), architectures(), latest_versions() { };
    /**
     * @brief Constructs a UbuntuRelease from a pair of release name and architectures.
     *
     * @param data A pair where the first element is the release name and the second is the list of supported architectures.
     */
    UbuntuRelease(const std::pair<std::string, std::vector<std::string>>& data):
        release_name(data.first), architectures(data.second), latest_versions() { };
    /**
     * @brief Compares two UbuntuRelease instances based on their release name.
     *
     * @param other Another UbuntuRelease to compare with.
     * @return True if this release name is lexicographically greater than the other's.
     */
    bool operator>(const UbuntuRelease& other) { return release_name > other.release_name; }
};
/**
 * @class UbuntuCloudInterface
 * @brief Abstract interface for fetching and querying Ubuntu cloud image metadata.
 */
class UbuntuCloudInterface {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~UbuntuCloudInterface() = default;

    /**
     * @brief Retrieves a list of all currently supported Ubuntu releases.
     *
     * @return A vector of UbuntuRelease objects representing supported releases.
     */
    virtual std::vector<UbuntuRelease> getSupportedReleases() const = 0;

    /**
     * @brief Retrieves the current Long Term Support (LTS) Ubuntu release, if available.
     *
     * @return An optional UbuntuRelease object for the current LTS version, or std::nullopt if not found.
     */
    virtual std::optional<UbuntuRelease> getCurrentLTS() const = 0;

    /**
     * @brief Retrieves the SHA256 checksum for the "disk1.img" of a given Ubuntu release for amd64, if available.
     *
     * @param release The name or title of the Ubuntu release to search for.
     * @return An optional string containing the SHA256 checksum, or std::nullopt if not available.
     */
    virtual std::optional<std::string> getSha256ForRelease(const std::string& release) const = 0;

    /**
     * @brief Indicates whether the implementation has been successfully initialized.
     *
     * @return True if the fetcher has valid data and is ready; false otherwise.
     */
    virtual bool isInitialized() const = 0;
};