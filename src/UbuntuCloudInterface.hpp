#pragma once

#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

struct UbuntuRelease {
    std::string              release_name;
    std::vector<std::string> architectures;
    UbuntuRelease();
    UbuntuRelease(const std::pair<std::string, std::vector<std::string>>& data);
    bool operator>(const UbuntuRelease& other) { return release_name > other.release_name; }
};

class UbuntuCloudInterface {
  public:
    virtual ~UbuntuCloudInterface() = default;

    // Return a list of all currently supported Ubuntu releases
    virtual std::vector<UbuntuRelease> getSupportedReleases() const = 0;

    // Return the current Ubuntu LTS version
    virtual std::optional<UbuntuRelease> getCurrentLTS() const = 0;

    // Return the sha256 of the disk1.img item of a given Ubuntu release
    virtual std::optional<std::string> getSha256ForRelease(const std::string& release) const = 0;

    virtual bool isInitialized() const = 0;
};