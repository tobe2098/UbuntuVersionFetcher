#include "UbuntuCloudIO.hpp"
/**
 * @file UbuntuCloudIO.cpp
 * @brief Provides user-facing output and helper functions for Ubuntu cloud release data.
 *
 * This file defines functions to:
 * - Display a list of supported Ubuntu releases and their architectures.
 * - Print information about the current Long Term Support (LTS) release.
 * - Retrieve and display the SHA256 checksum for a specific Ubuntu release.
 * - Show command-line usage instructions.
 *
 * The functions expect a valid, initialized `UbuntuCloudInterface` instance and handle
 * common output formatting, including structured indentation, table layouts, and console-friendly spacing.
 *
 * @note Functions in this file assume ownership of the provided `UbuntuCloudInterface` pointer via `std::unique_ptr` and move semantics.
 */

/// Base indentation level for all formatted output.
constexpr static int base_indentation { 1 };

/// Width of the left column for table output (architecture names).
constexpr static int table_width_left { 15 };

/// Width of the right column for table output (version information).
constexpr static int table_width_right { 10 };

// Declarations made with constexpr static to avoid macros with same visibility by linker (only this file).

std::string indentation(int indentation_level) {
  return std::string(2 * (indentation_level + base_indentation), ' ');
}

int printSupportedReleases(std::unique_ptr<UbuntuCloudInterface> fetcher) {
  std::vector<UbuntuRelease> releases { fetcher->getSupportedReleases() };
  // Call function with RVO in mind to avoid copies, guaranteed to be sorted (with non-LTS versions first).
  if (releases.empty()) {
    return 1;
  }
  bool LTS { false };  // Boolean to track if we are in the LTS versions.
  // Aesthetic newline.
  std::cout << '\n';
  std::cout << indentation(0) << "Supported Ubuntu releases:\n";
  std::cout << indentation(1) << "Non-LTS:\n";
  for (auto& [release_name, archs, version] : releases) {
    // Structured binding to avoid repetitive struct member access.
    if (!LTS && release_name.find("LTS") != std::string::npos) {
      // Because of sorted output, when we find the first LTS the rest of releases are LTS.
      LTS = true;
      std::cout << indentation(1) << "LTS:\n";
    }
    if (LTS) {
      // Remove LTS from the title as we know it is an LTS version from the boolean (aesthetics).
      size_t pos = release_name.find("LTS");
      if (pos != std::string::npos) {
        release_name.erase(pos, 4);
      }
    }
    std::cout << indentation(2) << release_name << " : ";
    for (const std::string& arch : archs) {
      // Print supported architectures.
      std::cout << arch << " ";
    }
    std::cout << "\n";  // Aesthetic newline.
  }
  return 0;
}

int printCurrentLTSRelease(std::unique_ptr<UbuntuCloudInterface> fetcher) {
  std::optional<UbuntuRelease> currentLTS { fetcher->getCurrentLTS() };
  if (!currentLTS) {
    // If optional is empty, no LTS release was found in data.
    return 1;
  }

  std::cout << '\n';
  // Aesthetic newline.
  std::cout << indentation(0) << "Current LTS version:";
  std::cout << indentation(0) << currentLTS->release_name << "\n\n";

  std::cout << indentation(1) << std::left  // left-align columns.
            << std::setw(15) << "Architecture"
            << " : " << std::setw(10) << "Version" << '\n';

  std::cout << indentation(1) << std::string(30, '-') << '\n';  // separator.

  size_t arch_size { currentLTS->architectures.size() };
  for (size_t arch = 0; arch < arch_size; arch++) {
    // Printing each version with its own architecture.
    std::cout << indentation(1) << std::left << std::setw(table_width_left) << currentLTS->architectures.at(arch) << " : "
              << std::setw(table_width_right) << currentLTS->latest_versions.at(arch) << '\n';
  }
  // Aesthetic newline.
  std::cout << "\n";
  return 0;
}

int printReleaseSHA256(std::unique_ptr<UbuntuCloudInterface> fetcher, const std::string& version) {
  std::optional<std::string> releaseSha256 { fetcher->getSha256ForRelease(version) };
  if (!releaseSha256) {
    std::cerr << "The release was not found";
    return 1;
  }
  std::cout << '\n'
            << indentation(0) << "The SHA256 of disk1.img in the most recent version of Ubuntu " << version << " is:\n"
            << indentation(0) << ">" << indentation(1) << *releaseSha256 << '\n';

  return 0;
}

void printHelp() {
  std::cout << '\n';
  std::cout << "Usage: ubuntu-version-fetcher [OPTIONS]\n"
            << "Options:\n"
            << "  --supported-releases                   List all supported Ubuntu releases\n"
            << "  --lts-version                          Show the current Ubuntu LTS version for each architecture\n"
            << "  --sha256 RELEASE_TITLE/RELEASE         Get the SHA256 hash of disk1.img for the last version of the specified release \n"
               "                                         for amd64. Use the release or titles as shown in --supported-releases\n"
               "                                          Examples:\n"
               "                                            ubuntu-version-fetcher --sha256 noble\n"
               "                                            ubuntu-version-fetcher --sha256 22.04\n"
            << "  --help                                 Display this help and exit\n";
  std::cout << '\n';
}
