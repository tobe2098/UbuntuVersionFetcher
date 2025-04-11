
#include <iostream>
#include "UbuntuCloudFactory.hpp"

constexpr int base_indentation { 1 };

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
  for (auto& [release_name, archs] : releases) {
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

void printHelp() {
  std::cout << "Usage: ubuntu-version-fetcher [OPTION]\n"
            << "Options:\n"
            << "  --supported-releases     List all supported Ubuntu releases\n"
            << "  --current-lts            Show the current Ubuntu LTS version\n"
            << "  --sha256 RELEASE         Get the SHA256 hash of disk1.img for the specified release\n"
            // << "  --url URL           Use custom Simplestreams URL\n"
            << "  --help                   Display this help and exit\n";
}

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);
  if (argc < 2) {
    printHelp();
    return 0;
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);
  std::string option = argv[1];

  std::unique_ptr<UbuntuCloudInterface> fetcher = UbuntuCloudFactory::createFetcher();
  // The unique_ptr is passed with move semantics because only one handler function should be called per runtime.
  // Contents are destroyed in the scope of the handler function.
  if (fetcher == nullptr) {
    std::cerr << "Failed to create cloud fetcher instance." << std::endl;
    return 1;
  }
  if (!fetcher->isInitialized()) {
    std::cerr << "Data could not be fetched." << std::endl;
    return 1;
  }
  int return_code { 0 };
  if (option == "--supported-releases") {
    return_code = printSupportedReleases(std::move(fetcher));
    if (return_code == 1) {
      std::cerr << "No supported Ubuntu releases were found.\n";
    }
  } else if (option == "--lts-version") {
  } else if (option == "--help") {
    printHelp();
  } else {
    std::cerr << "Unknown option: " << option << "\n";
    printHelp();
    return_code = 1;
  }
  // Include options:
  //   1- Changing the url (only if input with --url?)
  //   2- Return the list of all currently *supported* Ubuntu releases
  //   3- Return the current Ubuntu LTS version
  //   4- Return the sha256 of the disk1.img of a given Ubuntu release (string match)

  curl_global_cleanup();
  std::cout << "\n";
  return return_code;
}