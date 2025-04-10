
#include <iostream>
#include "UbuntuCloudFactory.hpp"

typedef std::unique_ptr<UbuntuCloudInterface> UCI_Ptr;

void printHelp() { }

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);

  if (argc < 2) {
    printHelp();
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);
  std::string option = argv[1];

  UCI_Ptr fetcher = UbuntuCloudFactory::createFetcher();
  if (fetcher == nullptr) {
    std::cerr << "Failed to create cloud fetcher instance.\n";
    return 1;
  }
  int return_code { 0 };
  if (option == "--list-releases") {
    std::vector<std::string> releases { fetcher->getSupportedReleases() };
    std::cout << "Supported Ubuntu releases: \n";
    for (std::string_view release : releases) {
      std::cout << release << '\n';
    }
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
  return return_code;
}