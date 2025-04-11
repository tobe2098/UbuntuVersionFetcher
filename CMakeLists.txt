cmake_minimum_required(VERSION 3.14)
project(UbuntuCloudInfo VERSION 1.0.0 LANGUAGES CXX)

#Find packages
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

#Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(SOURCES
    src/UbuntuCloudFetcher.cpp
    src/handlers.cpp
    src/main.cpp
)

set(HEADERS
    src/UbuntuCloudFetcher.hpp
    src/UbuntuCloudInterface.hpp
    src/UbuntuCloudFactory.hpp
    src/handlers.hpp
)

# Add executable
add_executable(ubuntu-version-fetcher ${SOURCES} ${HEADERS})

# Include directory
target_include_directories(ubuntu-version-fetcher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)


# Link libraries
target_link_libraries(ubuntu-version-fetcher PRIVATE 
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

# Compile options
target_compile_options(ubuntu-version-fetcher PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

install(TARGETS ubuntu-version-fetcher
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
# Testing (optional)
option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Generate compile_commands.json for tools like clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
