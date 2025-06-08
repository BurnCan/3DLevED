
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
mingw32-make
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
make
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
make
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

## ✅ You're good to go!

Happy coding! 🎮
