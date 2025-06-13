#!/bin/bash
set -e

# Resolve script path and name
SCRIPT_PATH=$(realpath "$0")
SCRIPT_NAME=$(basename "$SCRIPT_PATH")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# Check if we're in a temp dir — if so, auto-run Option 1
if [[ "$SCRIPT_DIR" == /tmp/* || "$SCRIPT_DIR" == /private/var/folders/* ]]; then
  OPTION="1"
else
  # Show interactive menu
  echo "========== 3DLevED Project Setup =========="
  echo "1. Clone from Repository and Compile"
  echo "2. Compile from local source code"
  echo "3. Rebuild after CMake configuration changes"
  echo "4. Remove temporary script files"
  echo "5. Exit"
  echo "==========================================="
  read -p "Choose an option [1-5]: " OPTION
fi

if [[ "$OPTION" == "1" ]]; then
  # Only copy to temp dir if not already running in one
  if [[ "$SCRIPT_DIR" != /tmp/* && "$SCRIPT_DIR" != /private/var/folders/* ]]; then
    TEMP_DIR=$(mktemp -d)
    cp "$SCRIPT_PATH" "$TEMP_DIR/"
    cd "$TEMP_DIR"
    exec "$TEMP_DIR/$SCRIPT_NAME"
  fi

  # Prompt for repo URL and branch
  read -p "Enter the git repository URL (e.g., https://github.com/yourusername/your-repo.git): " REPO_URL
  REPO_URL=$(echo "$REPO_URL" | xargs) # Trim whitespace
  REPO_URL=${REPO_URL:-"https://github.com/yourusername/your-repo.git"}

  read -p "Enter the branch name to clone (leave blank for default): " BRANCH_NAME
  BRANCH_NAME=$(echo "$BRANCH_NAME" | xargs)

  REPO_NAME=$(basename -s .git "$REPO_URL")
  TARGET_DIR="$HOME/$REPO_NAME"
  cd "$HOME"

  if [ -d "$TARGET_DIR" ]; then
    echo "[INFO] Removing existing directory $TARGET_DIR"
    rm -rf "$TARGET_DIR"
  fi

  if [[ -n "$BRANCH_NAME" ]]; then
    echo "[INFO] Cloning $REPO_URL (branch: $BRANCH_NAME)..."
    git clone --branch "$BRANCH_NAME" --single-branch "$REPO_URL"
  else
    echo "[INFO] Cloning $REPO_URL (default branch)..."
    git clone "$REPO_URL"
  fi

  cd "$TARGET_DIR"
  mkdir -p build
  cd build

  OS_NAME=$(uname)
  echo "[INFO] Detected OS: $OS_NAME"

  if [[ "$OS_NAME" == "Darwin" ]]; then
    echo "[INFO] Building for macOS..."
    cmake ..
    make -j4
  elif [[ "$OS_NAME" == "Linux" ]]; then
    echo "[INFO] Building for Linux..."
    cmake ..
    make -j4
  else
    echo "[INFO] Building for Windows (MinGW assumed)..."
    cmake -G "MinGW Makefiles" ..
    mingw32-make -j4
  fi

  # Cleanup temp script dir
  if [[ "$SCRIPT_DIR" == /tmp/* || "$SCRIPT_DIR" == /private/var/folders/* ]]; then
    echo "[INFO] Cleaning up temporary files in $SCRIPT_DIR"
    rm -rf "$SCRIPT_DIR"
  fi

  # Make script executable in the new project dir
  chmod +x "$TARGET_DIR/$SCRIPT_NAME"
  echo "[INFO] Made script executable at $TARGET_DIR/$SCRIPT_NAME"

  echo "[INFO] Build complete."
  echo "[INFO] Cleaning up temporary files..."
  rm -rf "$SCRIPT_DIR"
  echo "[INFO] Done. Project is located at $TARGET_DIR"

  # Ask if user wants to run the application after build
  echo
  read -p "Do you want to run the application now? [y/N]: " RUN_APP
  if [[ "$RUN_APP" =~ ^[Yy]$ ]]; then
    APP_DIR="$TARGET_DIR/build/bin"
    
    if [[ "$OS_NAME" == "Darwin" ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME.app/Contents/MacOS/$REPO_NAME"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR/$REPO_NAME.app/Contents/MacOS"
        echo "[INFO] Launching macOS application..."
        "$APP_EXEC"
      else
        echo "[ERROR] macOS application binary not found or not executable: $APP_EXEC"
      fi
    elif [[ "$OS_NAME" == "Linux" ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR"
        echo "[INFO] Launching Linux application..."
        "$APP_EXEC"
      else
        echo "[ERROR] Linux application binary not found or not executable: $APP_EXEC"
      fi
    elif [[ "$OS_NAME" == "CYGWIN"* || "$OS_NAME" == "MINGW"* ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME.exe"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR"
        echo "[INFO] Launching Windows application..."
        "$APP_EXEC"
      else
        echo "[ERROR] Windows application binary not found or not executable: $APP_EXEC"
      fi
    else
      echo "[ERROR] Unsupported OS: $OS_NAME. Please manually run the application."
    fi
  else
    cd ~
    echo "[INFO] Returned to home directory."
  fi

elif [[ "$OPTION" == "2" ]]; then
  REPO_NAME="3DLevED"
  TARGET_DIR="$HOME/$REPO_NAME"

  if [ ! -d "$TARGET_DIR" ]; then
    echo "[ERROR] Directory $TARGET_DIR not found."
    echo "Please run Option 1 first to clone the repository."
    exit 1
  fi

  echo "[INFO] Rebuilding project at $TARGET_DIR"
  cd "$TARGET_DIR"
  mkdir -p build
  cd build

  OS_NAME=$(uname)
  echo "[INFO] Detected OS: $OS_NAME"

  if [[ "$OS_NAME" == "Darwin" ]]; then
    echo "[INFO] Building for macOS..."
    cmake ..
    make -j4
  elif [[ "$OS_NAME" == "Linux" ]]; then
    echo "[INFO] Building for Linux..."
    cmake ..
    make -j4
  else
    echo "[INFO] Building for Windows (MinGW assumed)..."
    cmake -G "MinGW Makefiles" ..
    mingw32-make -j4
  fi

  echo "[INFO] Rebuild complete."

  # Ask if user wants to run the application after build
  echo
  read -p "Do you want to run the application now? [y/N]: " RUN_APP
  if [[ "$RUN_APP" =~ ^[Yy]$ ]]; then
    APP_DIR="$TARGET_DIR/build/bin"
    
    if [[ "$OS_NAME" == "Darwin" ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME.app/Contents/MacOS/$REPO_NAME"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR/$REPO_NAME.app/Contents/MacOS"
        echo "[INFO] Launching macOS application..."
        "$APP_EXEC"
      else
        echo "[ERROR] macOS application binary not found or not executable: $APP_EXEC"
      fi
    elif [[ "$OS_NAME" == "Linux" ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR"
        echo "[INFO] Launching Linux application..."
        "$APP_EXEC"
      else
        echo "[ERROR] Linux application binary not found or not executable: $APP_EXEC"
      fi
    elif [[ "$OS_NAME" == "CYGWIN"* || "$OS_NAME" == "MINGW"* ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME.exe"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR"
        echo "[INFO] Launching Windows application..."
        "$APP_EXEC"
      else
        echo "[ERROR] Windows application binary not found or not executable: $APP_EXEC"
      fi
    else
      echo "[ERROR] Unsupported OS: $OS_NAME. Please manually run the application."
    fi
  else
    cd ~
    echo "[INFO] Returned to home directory."
  fi

elif [[ "$OPTION" == "3" ]]; then
  REPO_NAME="3DLevED"
  TARGET_DIR="$HOME/$REPO_NAME"

  if [ ! -d "$TARGET_DIR" ]; then
    echo "[ERROR] Directory $TARGET_DIR not found."
    echo "Please run Option 1 first to clone the repository."
    exit 1
  fi

  echo "[INFO] Rebuilding after CMake configuration changes"
  cd "$TARGET_DIR/build"
  
  # Remove cached build files
  echo "[INFO] Removing CMake cache and build files..."
  rm CMakeCache.txt
  rm -rf CMakeFiles

  # Rebuild the project
  cmake ..
  make -j4

  echo "[INFO] Rebuild complete."

  # Ask if user wants to run the application after build
  echo
  read -p "Do you want to run the application now? [y/N]: " RUN_APP
  if [[ "$RUN_APP" =~ ^[Yy]$ ]]; then
    APP_DIR="$TARGET_DIR/build/bin"
    
    if [[ "$OS_NAME" == "Darwin" ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME.app/Contents/MacOS/$REPO_NAME"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR/$REPO_NAME.app/Contents/MacOS"
        echo "[INFO] Launching macOS application..."
        "$APP_EXEC"
      else
        echo "[ERROR] macOS application binary not found or not executable: $APP_EXEC"
      fi
    elif [[ "$OS_NAME" == "Linux" ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR"
        echo "[INFO] Launching Linux application..."
        "$APP_EXEC"
      else
        echo "[ERROR] Linux application binary not found or not executable: $APP_EXEC"
      fi
    elif [[ "$OS_NAME" == "CYGWIN"* || "$OS_NAME" == "MINGW"* ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME.exe"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR"
        echo "[INFO] Launching Windows application..."
        "$APP_EXEC"
      else
        echo "[ERROR] Windows application binary not found or not executable: $APP_EXEC"
      fi
    else
      echo "[ERROR] Unsupported OS: $OS_NAME. Please manually run the application."
    fi
  else
    cd ~
    echo "[INFO] Returned to home directory."
  fi

elif [[ "$OPTION" == "4" ]]; then
  # Same cleanup option as before...
fi

elif [[ "$OPTION" == "5" ]]; then
  echo "Exiting..."
  exit 0
fi
