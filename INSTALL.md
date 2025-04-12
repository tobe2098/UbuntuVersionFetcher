# Build instructions for Windows

## Prerequisites
### For libcurl
This package uses `libcurl`, a dynamic library from `curl`. This library is installed via `cmake`, but itself needs certain libraries to be present. If you have installed `Git`, it is possible that these dependencies are already in place. For more, go to [curl dependencies](https://curl.se/docs/libs.html).  

## Installing requirements
Installation via MSYS2 has been chosen to avoid problems when dealing with the installation and posterior runtime linking of `libcurl.dll`.
1. Install [MSYS2](https://www.msys2.org/).
2. Install `cmake`, `gcc` or `clang` and `make` or `ninja` from the `mingw64` terminal. Updating first is recommended.
```
pacman -Syu
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-clang
pacman -S mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-make
```
3. Add `C:\msys64\mingw64\bin` (or your MSYS2 installation path) to the PATH variable.
4. Run `cmake --version` to verify the installation. If there are any issues, it is recommended to run the command with the other installed commands. 

## Building this package from tarball
Unpack the tarball with 
```
tar -xf tarball.tar.xz
```
As of Windows 10 build 17063, `tar` is available in all systems.
## Building this package from github
Download the repository with
```
git clone https://github.com/tobe2098/UbuntuVersionFetcher
```
If you do not have Git, install it using the installer from [Git's page](https://git-scm.com/downloads)
## Build the binary
Simply run (if using `make`) 
```
cmake .
```
or if using `ninja`:
```
cmake . -G Ninja
```
Afterwards, 
```
ninja
```
or 
```
make
```
in the main directory (this folder).

## Run the binary
The binary is in hte `./bin/` folder. Use the `--help` option for more information on how to use it.