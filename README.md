
# 3DLevED (Cross-Platform Setup Guide)

This guide walks you through setting up your development environment and compiling this project on **Windows**, **macOS**, and **Ubuntu/Debian**.

---

## 🧰 Prerequisites

- Git
- CMake (3.14+)
- C++ Compiler (GCC, Clang, or MSVC)
- OpenGL-compatible GPU

---

## 🪟 Windows (MSYS2 + MinGW64)

### 1. Download and install the appropriate 64-bit version from the [MSYS2 website](https://www.msys2.org). 


### 2. Open MSYS2 MinGW 64-bit terminal

> Not `MSYS`, `UCRT`, or `CLANG`.

### 3. Update MSYS2

```bash
pacman -Syu
# Restart terminal if prompted
pacman -Su
```

### 4. Install tools

```bash
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-make git
```

### 5. Clone the repository

```bash
git clone https://github.com/BurnCan/3DLevED
cd 3DLevED
```

### 6. Build

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make -j4
```

### 7. Run

```bash
cd bin
./OpenGLApp.exe
```

---

## 🍏 macOS

### 1. Install Homebrew using the install script from the [Homebrew website](https://brew.sh)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
Post-installation steps
When you install Homebrew, it prints some directions for updating your shell’s config. If you don’t follow those directions, Homebrew will not work.

You need to update your shell’s config file (which file exactly depends on your shell, for example ~/.bashrc or ~/.zshrc) to include this:
```bash
eval "$(<Homebrew prefix path>/bin/brew shellenv)"
```
Replace <Homebrew prefix path> with the directory where Homebrew is installed on your system. You can find Homebrew’s default install location in [this FAQ entry.](https://docs.brew.sh/FAQ#why-should-i-install-homebrew-in-the-default-location) 


### 2. Install tools

```bash
brew install cmake git
```

### 3. Build

```bash
git clone https://github.com/BurnCan/3DLevED
cd 3DLevED
mkdir build && cd build
cmake ..
make -j4
```

### 4. Run

```bash
cd bin/OpenGLApp.app/Contents/MacOS
./OpenGLApp

```

---

## 🐧 Ubuntu / Debian Linux

### 1. Install tools

```bash
sudo apt update
sudo apt install build-essential cmake git libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev
```

### 2. Build

```bash
git clone https://github.com/BurnCan/3DLevED
cd 3DLevED
mkdir build && cd build
cmake ..
make -j4
```

### 3. Run

```bash
cd bin
./YourAppExecutableName
```

---

## 📦 Optional: Install libraries manually (Windows only)

```bash
pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-glew mingw-w64-x86_64-glm mingw-w64-x86_64-imgui
```

If you're using `FetchContent` in CMake, these are **not required**.

---

## 📁 Project Structure Example (windows)

```
OpenGLProjectTemplate/
├── shaders/
├── src/
├── CMakeLists.txt
├── README.md
└── build/
    └── bin/
	├── shaders/
        └── OpenGLApp.exe
```

---

#🛠️ Rebuilding the Project
Depending on your situation, choose one of the following options to rebuild the project.

✅ Option 1: Rebuild Normally (after code or shader edits)
```bash

cd build
```
OS-specific build command below
🪟Windows (MSYS2):

```bash
mingw32-make -j4
```
🍏🐧macOS / Linux:

```bash

make -j4
```
-j4 runs 4 compilation jobs in parallel. Adjust the number to match your CPU.

🔄 Option 2: Rebuild After CMake Configuration Changes
```bash
cd build
# Remove cached build files
rm CMakeCache.txt
rm -rf CMakeFiles
```
OS-specific cmake and build commands below
🪟Windows (MSYS2):

```bash

cmake -G "MinGW Makefiles" ..
mingw32-make -j4
```
🍏🐧macOS / Linux:

```bash

cmake ..
make -j4
```
🧹 Option 3: Full Clean — Delete and Reclone the Project
```bash
cd ..
rm -rf 3DLevED
git clone https://github.com/BurnCan/3DLevED
cd 3DLevED
mkdir build && cd build
```
OS-specific cmake and build commands below
🪟Windows (MSYS2):

```bash
cmake -G "MinGW Makefiles" ..
mingw32-make -j4
```
🍏🐧macOS / Linux:

```bash

cmake ..
make -j4
```
This deletes the entire project and gets a fresh copy from GitHub.

🗂️ Cloning a Specific Branch (Optional)
To clone a specific branch (e.g., development) instead of the default:

```bash
git clone --branch development --single-branch https://github.com/BurnCan/3DLevED
```
This clones only the specified branch, reducing download size and avoiding unrelated history.

---

#⚠️ Note on Working with Shaders:
The shaders/ directory is automatically copied into the build/bin/ directory by CMake. This ensures that the built application has access to the required shader files.

💡 Important: When modifying shaders, always edit them in the main project’s shaders/ directory, not in the build/bin/shaders/ copy. The files in build/bin are overwritten every time you re-run CMake or rebuild the project.

To apply your shader changes:

Make edits in shaders/ (in the project root).

Rebuild the project using make -j4 (or mingw32-make -j4 for windows).

This will ensure that your changes are properly copied into the app bundle and used at runtime.

## ✅ You're good to go!

Happy coding! 🎮
