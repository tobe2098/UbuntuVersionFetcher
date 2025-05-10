# UbuntuVersionFetcher
A C++ CLI that allows you to access the currently supported Ubuntu releases, the current LTS version and the sha256 of each release

# Build instructions
Go [here](./INSTALL.md)

# Usage
```
ubuntu-version-fetcher [OPTIONS]
            --supported-releases                   List all supported Ubuntu releases\n"
            --lts-version                          Show the current Ubuntu LTS version for each architecture\n"
            --sha256 RELEASE_TITLE/RELEASE         Get the SHA256 hash of disk1.img for the last version of the specified release \n"
                                                   for amd64. Use the release or titles as shown in --supported-releases\n"
                                                   Examples:\n"
                                                       ubuntu-version-fetcher --sha256 noble\n"
                                                       ubuntu-version-fetcher --sha256 22.04\n"
            --help                                 Display this help and exit\n";
```
