# Build instructions for Windows

## Prerequisites
### For libcurl
This package uses `libcurl`, a dynamic library from `curl`. This library is installed via `cmake`, but itself needs certain libraries to be present. If you have installed `Git`, it is possible that these dependencies are already in place. For more, go to [curl dependencies](https://curl.se/docs/libs.html).  
## Installing requirements via installers
### C++ compiler
You will need to have installed a C++ compiler. 
#### MSVC
1. Go to [Visual Studio Downloads](https://visualstudio.microsoft.com/downloads/).
2. Choose "Visual Studio Community" (free) or another edition if you have a license.

3. Run the installer, select "Desktop development with C++".
4. Complete the installation.
5. Verify the installation by running `cl` on the "Developer Command Prompt for VS".

#### Clang

1. Download the latest [LLVM installer](https://releases.llvm.org/) for Windows.
2. Run the installer and add choose LLVM to the system PATH.
3. Run `clang++ --version` to verify.

Ninja and CMake are required to install this package. All other dependencies are installed via cmake automatically.

### Build system
#### Ninja
1. Go to the GitHub releases page: https://github.com/ninja-build/ninja/releases.
2. Download the latest ninja-win.zip.

3. Extract the zip file.
4. Move the ninja.exe to a permanent location (e.g., C:\ninja).
5. Add that location to your PATH environment variable if it is not already there.

6. Run `ninja --version` in the command prompt to verify the installation.
#### Make
- Make comes as a default build system in most Linux/macOS.
- Follow the installation instructions of [Make for Windows](https://gnuwin32.sourceforge.net/packages/make.htm).
### CMake
1. Visit the official CMake website: https://cmake.org/download/.
2. Download the latest Windows installer (.msi).

3. Choose "Add CMake to the system PATH" during installation.
4. Complete the installation.

5. Run `cmake --version` in the command prompt to verify the installation.

## Installing requirements via MSYS2
1. Install [MSYS2](https://www.msys2.org/).
2. Install `cmake`, `gcc` or `clang` and `make` or `ninja` from the `mingw64` terminal.
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
## Building this package from github

Simply run 
```
cmake .
```
and then 
```
ninja
```
or 
```
make
```
in the main directory (this folder).

Then run the executable from `./bin/`