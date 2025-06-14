
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
cd bin/3DLevED.app/Contents/MacOS
./3DLevED

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

##🛠️ Setting Up and recompiling/rebuilding 3DLevED

###⚠️We recommend using the provided setup script to manage cloning, building, and running the project.

📥 Step 1: Clone the Repository
```bash
git clone https://github.com/BurnCan/3DLevED
cd 3DLevED
```
🚀 Step 2: Make the Script Executable and Run It
```bash
chmod +x setup.sh && ./setup.sh
```
This launches an interactive menu that helps you build, rebuild, recompile, or run the application.

###🛠️ Manual Setup (Alternative)
If you prefer not to use the setup script, follow the appropriate manual instructions below:

🔄 Clone and build the Project
```bash
git clone https://github.com/BurnCan/3DLevED
cd 3DLevED
mkdir build && cd build
```

💡 To clone a specific branch (e.g., development):

```bash
git clone --branch development --single-branch https://github.com/BurnCan/3DLevED
```



OS-specific CMake and build commands:

🪟 Windows (MSYS2):

```bash

cmake -G "MinGW Makefiles" ..
mingw32-make -j4
```
🍏🐧 macOS / Linux:

```bash
cmake ..
make -j4
```
Run the application:

🪟/🐧 Windows / Linux:

```bash
./build/bin/3DLevED
```
🍏macOS:

```bash
./build/bin/3DLevED.app/Contents/MacOS/3DLevED
```
🔄  Recompile (After Code or Shader Edits)
```bash
cd build
```
OS-specific build commands:

🪟 Windows (MSYS2):

```bash
cmake -G "MinGW Makefiles" ..
mingw32-make -j4
```
🍏🐧 macOS / Linux:

```bash
cmake ..
make -j4
```
Run the application:

🪟/🐧Windows / Linux:

```bash
./build/bin/3DLevED
```

🍏macOS:

```bash
./build/bin/3DLevED.app/Contents/MacOS/3DLevED
```
🔄 Rebuild After CMake Configuration Changes
```bash
cd build
rm CMakeCache.txt
rm -rf CMakeFiles
```
OS-specific CMake and build commands:

🪟 Windows (MSYS2):

```bash
cmake -G "MinGW Makefiles" ..
mingw32-make -j4
```
🍏🐧 macOS / Linux:

```bash
cmake ..
make -j4
```
Run the application:

🪟/🐧Windows / Linux:

```bash
./build/bin/3DLevED
```
🍏macOS:

```bash
./build/bin/3DLevED.app/Contents/MacOS/3DLevED
```
 


🧹Remove the cloned project directory:

Assuming you cloned into the home directory
```bash
cd ~
```
then
```bash
rm -rf 3DLevED
```


🧹Remove temporary copies of script:

⚠️This does not yet work to remove temporary script files, any undeleted temporary script files (rebuild.sh) are in a directory within
/tmp for windows and linux or /private/var/folders however finding this directory manually can be quite difficult as the script does not yet copy itself to a temporary directory named 3DLevED but a directory named similar to /tmp/tmp.9N7hV6vCbu
Under certain circumstances these files and directories are not automatically deleted  after the script is run , to be sure you have deleted all temporary script files run the script and select option 4 which will scan /tmp and /private/var/folders for a file named rebuild.sh and prompt you for deletion. this will be fixed in a future update.

```bash
find /tmp -type d -name "3DLevED*" -exec rm -rf {} +
find /private/var/folders -type d -name "3DLevED*" -exec rm -rf {} +
```
This cleans up any leftover setup script directories.





---

## ⚠️ Note on Working with Shaders:
The shaders/ directory is automatically copied into the build/bin/ directory by CMake. This ensures that the built application has access to the required shader files.

💡 **Important: When modifying shaders, always edit them in the main project’s shaders/ directory, not in the build/bin/shaders/ copy. The files in build/bin are overwritten every time you re-run CMake or rebuild the project.**

To apply your shader changes:

Make edits in shaders/ (in the project root).

Recompile the project as described above.

This will ensure that your changes are properly copied into the app bundle and used at runtime.

## ✅ You're good to go!

Happy coding! 🎮
