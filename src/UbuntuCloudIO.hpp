/**
 * @file UbuntuCloudIO.hpp
 * @brief Provides functions for formatted console output of Ubuntu release information.
 *
 * This file separates input/output logic from application logic to improve the readability of main.cpp.
 * It defines utility functions for printing supported releases, the current LTS release,
 * SHA256 hashes for specific releases, and usage instructions for the tool.
 *
 * Functions in this file depend on an implementation of the UbuntuCloudInterface
 * to retrieve Ubuntu release metadata.
 */
#pragma once

#include "UbuntuCloudInterface.hpp"

#include <iostream>

// This file is to clean up main.cpp by separating most of the I/O work.

/**
 * @brief Generates a string of whitespace for indentation.
 *
 * @param indentation_level Relative indentation level.
 * @return std::string String of indentation whitespace.
 */
std::string indentation(int indentation_level);

/**
 * @brief Prints the supported Ubuntu releases and their architectures.
 *
 * Fetches supported Ubuntu releases from the provided cloud interface and
 * displays them in a formatted output, separating LTS and non-LTS versions.
 * The output is organized with non-LTS releases first, followed by LTS releases.
 * For each release, its name and supported architectures are displayed.
 *
 * @param fetcher Smart pointer to a UbuntuCloudInterface that provides release information.
 *                Ownership is transferred to this function, and the pointer will be invalidated after the call.
 * @return int Status code: 0 for success, 1 if no releases were found.
 *
 * @note Assumes the releases returned by fetcher->getSupportedReleases() are sorted
 *       with non-LTS versions appearing before LTS versions.
 *
 * @note Writes formatted output to std::cout on success.
 * @note Writes error message to std::cerr if release is not found.
 */
int printSupportedReleases(std::unique_ptr<UbuntuCloudInterface> fetcher);
/**
 * @brief Prints information about the current Ubuntu LTS release.
 *
 * Fetches the current LTS (Long Term Support) Ubuntu release from the provided cloud
 * interface and displays its name, supported architectures, and corresponding version
 * numbers in a formatted table.
 *
 * @param fetcher Smart pointer to a UbuntuCloudInterface that provides release information
 *                Ownership is transferred to this function, and the pointer will be invalidated after the call.
 * @return int Status code: 0 for success, 1 if no LTS release is found.
 *
 * @note The output is formatted as a table with architectures aligned to the left
 *       and version numbers displayed in corresponding columns.
 *
 * @note Writes formatted output to std::cout on success.
 * @note Writes error message to std::cerr if release is not found.
 */
int printCurrentLTSRelease(std::unique_ptr<UbuntuCloudInterface> fetcher);
/**
 * @brief Prints the SHA256 hash for a specified Ubuntu release version for amd64.
 *
 * Fetches and displays the SHA256 hash of disk1.img for the requested Ubuntu version.
 * The function queries the provided cloud interface to retrieve the hash information.
 *
 * @param fetcher Smart pointer to a UbuntuCloudInterface that provides release information.
 *                    Ownership is transferred to this function, and the pointer will be invalidated after the call.
 * @param version The Ubuntu version string to query (e.g., "22.04" or "23.10")
 * @return int Status code: 0 for success, 1 if the specified release was not found
 *
 * @note Writes formatted output to std::cout on success.
 * @note Writes error message to std::cerr if release is not found.
 */
int printReleaseSHA256(std::unique_ptr<UbuntuCloudInterface> fetcher, const std::string& version);
/**
 * @brief Displays the help message with usage instructions for the ubuntu-version-fetcher tool.
 *
 * Prints a formatted help message to standard output that lists all available command line
 * options for the ubuntu-version-fetcher tool, along with descriptions and examples of usage.
 *
 * @note Writes formatted help text to std::cout.
 *
 * The following options are documented in the help message:
 * - --supported-releases: List all supported Ubuntu releases.
 * - --lts-version: Show the current Ubuntu LTS version for each architecture.
 * - --sha256: Get the SHA256 hash for a specified Ubuntu release.
 * - --help: Display the help message.
 *
 */
void printHelp();