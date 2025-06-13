#!/bin/bash
set -e

# Resolve script path and name
SCRIPT_PATH=$(realpath "$0")
SCRIPT_NAME=$(basename "$SCRIPT_PATH")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# Default project repo name
REPO_NAME="3DLevED"
TARGET_DIR="$HOME/$REPO_NAME"

# Check if we're in a temp dir — if so, auto-run Option 1
if [[ "$SCRIPT_DIR" == /tmp/* || "$SCRIPT_DIR" == /private/var/folders/* ]]; then
  OPTION="1"
else
  # Show interactive menu
  echo "========== 3DLevED Project Setup =========="
  echo "1. Clone from Repository and Compile"
  echo "2. Recompile project"
  echo "3. Rebuild After CMake Configuration Changes"
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

  read -p "Enter the git repository URL (e.g., https://github.com/yourusername/your-repo.git): " REPO_URL
  REPO_URL=$(echo "$REPO_URL" | xargs)
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
  mkdir -p build && cd build

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

  # Make the script in target dir executable
  chmod +x "$TARGET_DIR/$SCRIPT_NAME"
  echo "[INFO] Made script executable at $TARGET_DIR/$SCRIPT_NAME"

  # Prompt to run app
  echo "[DEBUG] Prompting to run the application..."
  echo
  echo -n "Do you want to run the application now? [y/N]: "
  read RUN_APP
  if [[ "$RUN_APP" == "y" || "$RUN_APP" == "Y" ]]; then
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
    elif [[ "$OS_NAME" == CYGWIN* || "$OS_NAME" == MINGW* ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME.exe"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR"
        echo "[INFO] Launching Windows application..."
        "$APP_EXEC"
      else
        echo "[ERROR] Windows application binary not found or not executable: $APP_EXEC"
      fi
    else
      echo "[ERROR] Unsupported OS: $OS_NAME"
    fi
  else
    cd ~
    echo "[INFO] Returned to home directory."
  fi

  # Cleanup temp script dir
  if [[ "$SCRIPT_DIR" == /tmp/* || "$SCRIPT_DIR" == /private/var/folders/* ]]; then
    echo "[INFO] Cleaning up temporary files in $SCRIPT_DIR"
    rm -rf "$SCRIPT_DIR"
  fi

elif [[ "$OPTION" == "2" || "$OPTION" == "3" ]]; then
  if [ ! -d "$TARGET_DIR" ]; then
    echo "[ERROR] Directory $TARGET_DIR not found. Run Option 1 first."
    exit 1
  fi

  echo "[INFO] Rebuilding project at $TARGET_DIR"
  
  # Change directory to the target and clean if rebuilding
  cd "$TARGET_DIR"
  
  mkdir -p build && cd build

  if [[ "$OPTION" == "3" ]]; then
    echo "[INFO] Clearing CMake cache for reconfiguration..."
    rm -f CMakeCache.txt
    rm -rf CMakeFiles
  fi

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
  
  # Prompt to run app
  echo "[DEBUG] Prompting to run the application..."
  echo
  echo -n "Do you want to run the application now? [y/N]: "
  read RUN_APP
  if [[ "$RUN_APP" == "y" || "$RUN_APP" == "Y" ]]; then
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
    elif [[ "$OS_NAME" == CYGWIN* || "$OS_NAME" == MINGW* ]]; then
      APP_EXEC="$APP_DIR/$REPO_NAME.exe"
      if [[ -x "$APP_EXEC" ]]; then
        cd "$APP_DIR"
        echo "[INFO] Launching Windows application..."
        "$APP_EXEC"
      else
        echo "[ERROR] Windows application binary not found or not executable: $APP_EXEC"
      fi
    else
      echo "[ERROR] Unsupported OS: $OS_NAME"
    fi
  else
    cd ~
    echo "[INFO] Returned to home directory."
  fi

elif [[ "$OPTION" == "4" ]]; then
  echo "[INFO] Searching for temporary script files..."
  FOUND_DIRS=()
  SEARCH_DIRS=(/tmp /private/var/folders)
  for DIR in "${SEARCH_DIRS[@]}"; do
    echo "Searching in: $DIR"
    while IFS= read -r -d '' MATCH; do
      FOUND_DIRS+=("$(dirname "$MATCH")")
    done < <(find "$DIR" -type f -name "$SCRIPT_NAME" -print0 2>/dev/null)
  done

  if [[ ${#FOUND_DIRS[@]} -eq 0 ]]; then
    echo "No Temporary script files found."
    exit 0
  fi

  echo
  echo "Found the following temporary script directories:"
  for i in "${!FOUND_DIRS[@]}"; do
    echo "$((i+1)). ${FOUND_DIRS[$i]}"
  done

  echo
  echo "1. Delete all"
  echo "2. Delete individually"
  echo "3. Cancel and return to main menu"
  read -p "Choose an option [1-3]: " DELETE_OPTION

  if [[ "$DELETE_OPTION" == "1" ]]; then
    for dir in "${FOUND_DIRS[@]}"; do
      echo "Deleting $dir"
      rm -rf "$dir"
    done
    echo "All temporary script directories removed."
  elif [[ "$DELETE_OPTION" == "2" ]]; then
    for dir in "${FOUND_DIRS[@]}"; do
      read -p "Delete $dir? [y/N]: " CONFIRM
      if [[ "$CONFIRM" =~ ^[Yy]$ ]]; then
        rm -rf "$dir"
        echo "Deleted $dir"
      else
        echo "Skipped $dir"
      fi
    done
  else
    echo "Cancelled. Returning to main menu."
    exec "$SCRIPT_PATH"
  fi

elif [[ "$OPTION" == "5" ]]; then
  echo "Exiting..."
  exit 0

else
  echo "Invalid selection. Exiting."
  exit 1
fi
