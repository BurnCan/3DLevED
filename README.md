
# 3DLevED (Cross-Platform Setup Guide)

This guide walks you through setting up your development environment and compiling this project on **Windows**, **macOS**, and **Ubuntu/Debian**.

---

## 🧰 Prerequisites
Before building or running 3DLevED, make sure your system has the required tools installed.

✅ Required on All Platforms
Git – For cloning the repository

CMake (3.14 or higher) – For project configuration

C++ Compiler

GCC, Clang, or MSYS2/MinGW depending on OS

OpenGL-Compatible GPU

---
## OS Specific installation instructions for prerequisite tools:
---
## 🪟 Windows (MSYS2 + MinGW64)

### 1. Download and install the appropriate 64-bit version from the [MSYS2 website](https://www.msys2.org). 


### 2. Open MSYS2 UCRT 64-bit terminal

> Not `MSYS`, `MinGW`, or `CLANG`.

### 3. Update the package database and core system
```bash
pacman -Syu
# Restart terminal if prompted
pacman -Su
```
### 4. Install build tools
```bash
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-make git
```
📦 Optional: Install libraries manually 
Example:
```bash
pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-glew mingw-w64-x86_64-glm mingw-w64-x86_64-imgui
```

If the libraries you want to install are included in `FetchContent` in CMake, this is **not required**.

---
## 🍏 macOS

### 1. Install Homebrew using the install script from the [Homebrew website](https://brew.sh)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
💡 Important: Follow the post-install instructions to add Homebrew to your shell config.

Post-installation steps
When you install Homebrew, it prints some directions for updating your shell’s config. If you don’t follow those directions, Homebrew will not work.

You need to update your shell’s config file (which file exactly depends on your shell, for example ~/.bashrc or ~/.zshrc) to include this
Example:

```bash
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
```

Replace <Homebrew prefix path> with the directory where Homebrew is installed on your system. You can find Homebrew’s default install location in [this FAQ entry.](https://docs.brew.sh/FAQ#why-should-i-install-homebrew-in-the-default-location) 


### 2. Install tools

```bash
brew install cmake git
```


---

## 🐧 Ubuntu / Debian Linux

### 1. Install tools

```bash
sudo apt update
sudo apt install build-essential cmake git libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev
```


---




## 🛠️ Setting Up and recompiling/rebuilding 3DLevED

### ⚠️We recommend using the provided setup script to manage cloning, building, and running the project.

📥 Step 1: Clone the Repository
```bash
git clone https://github.com/BurnCan/3DLevED
cd 3DLevED
```
🚀 Step 2: Make the Script Executable and Run It
```bash
chmod +x build.sh && ./build.sh
```
This launches an interactive menu that helps you build, rebuild, recompile, or run the application.

⚠️Note that if using option 1 to clone the repository you will likely need to make the script executable again before you will be able to run the script. If you see an error like "No Such File Found" when attempting to use ./build.sh run the command in step 2 above.

---
### 🛠️ Manual Setup (Alternative)
---
If you prefer not to use the setup script, follow the appropriate manual instructions below:

---
🔄 Clone and build the Project
---
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

---
---
🔄  Recompile (After Code or Shader Edits)
---
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
 
---
Uninstall instructions
---

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

⚠️This may not yet work to remove temporary script files, any undeleted temporary script files (rebuild.sh) are in a directory within
/tmp for windows and linux or /private/var/folders however finding this directory manually can be quite difficult if you had previously run an older version as the script did not yet copy itself to a temporary directory named 3DLevED but a directory named similar to /tmp/tmp.9N7hV6vCbu
Under certain circumstances these files and directories are not automatically deleted  after the script is run , to be sure you have deleted all temporary script files run the script and select option 4 which will scan /tmp and /private/var/folders for a file named rebuild.sh or build.sh and prompt you for deletion. The command below will only remove script files in the newer /tmp/3DLevED directories and remains untested.

```bash
find /tmp -type d -name "3DLevED*" -exec rm -rf {} +
find /private/var/folders -type d -name "3DLevED*" -exec rm -rf {} +
```
This cleans up any leftover setup script directories.





---

## 📁 Project Structure Example (windows)

```
3DLevED/
├── shaders/
├── src/
├── CMakeLists.txt
├── README.md
├── rebuild.sh
└── build/
    └── bin/
	├── shaders/
        └── 3DLevED.exe
```


## ⚠️ Working with Shaders in 3DLevED

The `archive/` directory contains all runtime assets needed by the application, including the `shaders/` subdirectory.
During a build, CMake **recursively copies the contents of `archive/`** into the appropriate runtime location:

- **macOS:** `build/3DLevED.app/Contents/Resources/`
- **Linux/Windows:** `build/bin/`

This ensures that your application has access to all necessary shader files and supporting data.

---

### 💡 Shader Editing Workflow

To modify and maintain shader files properly, follow this workflow:

#### 🎮 Runtime Shader Location
- **Live shaders are loaded from:**  

build/bin/shaders/

(or inside the app bundle on macOS)

- When the application runs, it reads shaders from this location.

#### ✏️ Editing Shaders
- Use the in-app **Shader Utility Editor** in 3DLevED:
- The **Save** button updates the file in `build/bin/shaders/`.
- The **Reload** button re-reads the file from `build/bin/shaders/`.

> ✅ **Important:** Always make shader edits through the in-app editor (or directly in `build/bin/shaders/`) — changes in the original `archive/shaders/` directory will be **overwritten** next time you rebuild the project.

#### 📦 Archiving Shader Changes
- To persist a modified shader file:
- Use the **Archive** button in the Shader Utility.
- This saves the updated shader file back to:
  
  archive/shaders/
  
- On the next build, this updated file will be included in the runtime copy.

---

## ✅ You're good to go!

Happy coding! 🎮
