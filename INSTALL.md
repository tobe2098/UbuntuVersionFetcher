# Installing requirements for Windows
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
3. Add `C:\msys64\mingw64\bin` (or your MSYS2 installation path) to the PATH variable (optional, only if you want to have access to the toolkit outside the `msys64` environment).
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
If you do not have Git, install it using the installer from [Git's page](https://git-scm.com/downloads) or install the `cygwin` version using:
```
pacman -S git
```



# Installing requirements for Linux
From a fresh install of Linux Mint, the following steps worked seamlessly.
## Steps
- Start by updating your system if possible:
```
sudo apt-get update
```
- Then we will install git (optional):
```
sudo apt install git
```
- Now we can install `cmake`:
```
sudo apt install cmake
```
- The project uses the `libcurl` developer header and library, so you will need to install it with the following:
```
sudo apt install libcurl4-openssl-dev 
```
- Now, we can unpack with `tar` the tarball or download the repository with `git clone`.
- Then we do `cmake .` and `make`. 

# Installing requirements for mac OS

Sadly I had no access to a mac OS environment, so refer to the instructions in linux. As long as `cmake`, a build system and a C++ compiler are installed, and the `libcurl` developer headers and libraries are installed and in the PATH variable, the installation and runtime should work.


# Build the binary
Simply run (if using `make`) 
```
cmake .
```
or if using `ninja` you may need to do:
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
If your `cmake` configuration has been changed, the build files may be inside `./build/`. In that case, `cd` into that folder and use your build system. The binary will also be linked in the `./build/bin/` folder. 
## Run the binary
The binary is in the `./bin/` folder. Use the `--help` option for more information on how to use it.

