#pragma once


#include <vector>
#include <string>
#include <optional>

class UbuntuCloudInterface {
    public:
        virtual ~UbuntuCloudInterface() = default;
        
        // Return a list of all currently supported Ubuntu releases
        virtual std::vector<std::string> getSupportedReleases() const = 0;
        
        // Return the current Ubuntu LTS version
        virtual std::string getCurrentLTS() const = 0;
        
        // Return the sha256 of the disk1.img item of a given Ubuntu release
        virtual std::optional<std::string> getSha256ForRelease(const std::string& release) const = 0;
        
        // Refresh data from the source
        virtual bool refreshData() = 0;
    };