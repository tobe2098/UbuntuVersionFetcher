
#include "UbuntuCloudFactory.hpp"
#include "handlers.hpp"

#include <iostream>

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
    return_code = printCurrentLTSRelease(std::move(fetcher));
  } else if (option == "--sha256") {
    if (argc < 3) {
      std::cerr << "This option requires an additional argument";
      return_code = 1;
    } else {
      std::string releaseArg(argv[2]);
      return_code = printReleaseSHA256(std::move(fetcher), releaseArg);
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
  std::cout << "\n";
  return return_code;
}