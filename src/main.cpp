/**
 * @file main.cpp
 * @brief Entry point for the Ubuntu version fetcher tool.
 *
 * This file contains the `main()` function, which parses command-line arguments
 * and delegates functionality to specific handlers based on the provided options.
 *
 * Supported options:
 * - `--help`: Displays usage instructions and exits.
 * - `--supported-releases`: Prints all supported Ubuntu releases and their architectures.
 * - `--lts-version`: Prints the current Ubuntu LTS release and associated metadata.
 * - `--sha256 <release>`: Prints the SHA256 checksum for a specific Ubuntu release (amd64 architecture).
 *
 * The application uses libcurl to fetch release metadata in JSON format from the Ubuntu
 * cloud image server, and delegates parsing to an instance of `UbuntuCloudFetcher`.
 *
 * @note Requires network access to retrieve remote data.
 * @note Uses `UbuntuCloudInterface` polymorphically to separate concerns between fetching and displaying.
 * @note Initializes and cleans up libcurl in the program's lifetime.
 */

#include "UbuntuCloudFactory.hpp"
#include "UbuntuCloudIO.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);
  // Avoid sychronizing with the C I/O buffers for faster speed. Not very relevant here, but this should be in every program that does not
  // have multi-threaded I/O.
  if (argc < 2) {
    // If less than two arguments, print the help.
    printHelp();
    return 0;
  }
  std::string option = argv[1];  // If we are here, guaranteed at least one argument.
  if (option == "--help") {
    // Check and return early to avoid fetcher creation, print help.
    printHelp();
    return 0;
  }
  if (option == "--sha256") {
    if (argc < 3) {
      // We check now to avoid wasteful download and parsing.
      std::cerr << "This option requires an additional argument";
      return 1;
    }
  }
  curl_global_init(CURL_GLOBAL_DEFAULT);
  // initialize curl.

  std::unique_ptr<UbuntuCloudInterface> fetcher = UbuntuCloudFactory::createUbuntuVersionFetcher();
  // The unique_ptr is passed with move semantics because only one handler function should be called per runtime.
  // Contents are destroyed in the scope of the handler function.
  if (fetcher == nullptr) {
    // Failure to create the fetcher.
    std::cerr << "Failed to create cloud fetcher instance." << std::endl;
    return 1;
  }
  if (!fetcher->isInitialized()) {
    // Failure to retrieve and properly parse the data. std::cerr will have more information from ::fetchData().
    std::cerr << "Data could not be fetched." << std::endl;
    return 1;
  }
  int return_code { 0 };  // To avoid code duplication in cleanup.
  if (option == "--supported-releases") {
    // Call handler function moving the fetcher pointer there. Fetcher pointer is null.
    return_code = printSupportedReleases(std::move(fetcher));
    if (return_code == 1) {
      std::cerr << "No supported Ubuntu releases were found.\n";
    }
  } else if (option == "--lts-version") {
    // Call handler function moving the fetcher pointer there. Fetcher pointer is null.
    return_code = printCurrentLTSRelease(std::move(fetcher));
  } else if (option == "--sha256") {
    // From previous check, this argument is guaranteed to exist.
    std::string releaseArg(argv[2]);
    // Call handler function moving the fetcher pointer there. Fetcher pointer is null.
    return_code = printReleaseSHA256(std::move(fetcher), releaseArg);

  } else {
    // Handle any argument that does not fit.
    std::cerr << "Unknown option: " << option << "\n";
    // And print help.
    printHelp();
    return_code = 1;
  }
  // Aesthetics newline.
  std::cout << "\n";
  // Clean-up function.
  curl_global_cleanup();
  return return_code;
}