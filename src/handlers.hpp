#pragma once

#include "UbuntuCloudInterface.hpp"

#include <iostream>

std::string indentation(int indentation_level);

int printSupportedReleases(std::unique_ptr<UbuntuCloudInterface> fetcher);
int printCurrentLTSRelease(std::unique_ptr<UbuntuCloudInterface> fetcher);
int printReleaseSHA256(std::unique_ptr<UbuntuCloudInterface> fetcher, const std::string& version);

void printHelp();