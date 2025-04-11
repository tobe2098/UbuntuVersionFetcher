#include "handlers.hpp"

constexpr static int base_indentation { 1 };
constexpr static int table_width_left { 15 };
constexpr static int table_width_right { 10 };

std::string indentation(int indentation_level) {
  return std::string(2 * (indentation_level + base_indentation), ' ');
}

int printSupportedReleases(std::unique_ptr<UbuntuCloudInterface> fetcher) {
  std::vector<UbuntuRelease> releases { fetcher->getSupportedReleases() };
  if (releases.empty()) {
    return 1;
  }
  bool LTS { false };
  std::cout << '\n';
  std::cout << indentation(0) << "Supported Ubuntu releases:\n";
  std::cout << indentation(1) << "Non-LTS:\n";
  for (auto& [release_name, archs, version] : releases) {
    if (!LTS && release_name.find("LTS") != std::string::npos) {
      LTS = true;
      std::cout << indentation(1) << "LTS:\n";
    }
    if (LTS) {
      size_t pos = release_name.find("LTS");
      if (pos != std::string::npos) {
        release_name.erase(pos, 4);
      }
    }
    std::cout << indentation(2) << release_name << " : ";
    for (const std::string& arch : archs) {
      std::cout << arch << " ";
    }
    std::cout << "\n";
  }
  return 0;
}

int printCurrentLTSRelease(std::unique_ptr<UbuntuCloudInterface> fetcher) {
  std::optional<UbuntuRelease> currentLTS { fetcher->getCurrentLTS() };
  if (!currentLTS) {
    return 1;
  }

  std::cout << '\n';
  std::cout << indentation(0) << "Current LTS version:";
  std::cout << indentation(0) << currentLTS->release_name << "\n\n";

  std::cout << indentation(1) << std::left  // left-align columns
            << std::setw(15) << "Architecture"
            << " : " << std::setw(10) << "Version" << '\n';

  std::cout << indentation(1) << std::string(30, '-') << '\n';  // separator

  size_t arch_size { currentLTS->architectures.size() };
  for (size_t arch = 0; arch < arch_size; arch++) {
    std::cout << indentation(1) << std::left << std::setw(table_width_left) << currentLTS->architectures.at(arch) << " : "
              << std::setw(table_width_right) << currentLTS->latest_versions.at(arch) << '\n';
  }
  std::cout << "\n";
  return 0;
}

void printHelp() {
  std::cout << "Usage: ubuntu-version-fetcher [OPTION]\n"
            << "Options:\n"
            << "  --supported-releases     List all supported Ubuntu releases\n"
            << "  --current-lts            Show the current Ubuntu LTS version\n"
            << "  --sha256 RELEASE         Get the SHA256 hash of disk1.img for the specified release\n"
            // << "  --url URL           Use custom Simplestreams URL\n"
            << "  --help                   Display this help and exit\n";
}
