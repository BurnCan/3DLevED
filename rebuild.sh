#!/bin/bash
set -e

# Detect script path and name
SCRIPT_PATH=$(realpath "$0")
SCRIPT_NAME=$(basename "$SCRIPT_PATH")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# If running from a temporary directory, skip the menu and run clean install
if [[ "$SCRIPT_DIR" == /tmp/* || "$SCRIPT_DIR" == /private/var/folders/* ]]; then
  AUTO_CLEAN_INSTALL=true
else
  AUTO_CLEAN_INSTALL=false
fi

# Display menu only if not in temp directory
if [[ "$AUTO_CLEAN_INSTALL" == false ]]; then
  echo "========== 3DLevED Project Setup =========="
  echo "1. Clean install from repository"
  echo "2. Placeholder Option 2"
  echo "3. Placeholder Option 3"
  echo "4. Remove temporary script files"
  echo "5. Exit"
  echo "==========================================="

  read -p "Choose an option [1-5]: " CHOICE
else
  CHOICE=1
fi

# Option 1: Clean install from repository
if [[ "$CHOICE" == "1" ]]; then
  # Avoid infinite recursion
  if [[ "$SCRIPT_DIR" != /tmp/* && "$SCRIPT_DIR" != /private/var/folders/* ]]; then
    TEMP_DIR=$(mktemp -d)
    cp "$SCRIPT_PATH" "$TEMP_DIR/"
    cd "$TEMP_DIR"
    exec "$TEMP_DIR/$SCRIPT_NAME"
  fi

  # Prompt for repo URL and auto-trim
  read -p "Enter the git repository URL (e.g., https://github.com/yourusername/your-repo.git): " REPO_URL
  REPO_URL=$(echo "$REPO_URL" | xargs)

  # Prompt for branch name (optional) and auto-trim
  read -p "Enter the branch name to clone (leave blank for default): " BRANCH_NAME
  BRANCH_NAME=$(echo "$BRANCH_NAME" | xargs)

  # Extract repo name from URL
  REPO_NAME=$(basename -s .git "$REPO_URL")

  # Target directory one level up from original location
  TARGET_DIR="$HOME/$REPO_NAME"
  cd "$HOME"

  # Remove existing repo if it exists
  if [ -d "$TARGET_DIR" ]; then
    echo "[INFO] Removing existing directory $TARGET_DIR"
    rm -rf "$TARGET_DIR"
  fi

  # Clone repo
  if [[ -n "$BRANCH_NAME" ]]; then
    echo "[INFO] Cloning $REPO_URL (branch: $BRANCH_NAME)..."
    git clone --branch "$BRANCH_NAME" --single-branch "$REPO_URL"
  else
    echo "[INFO] Cloning $REPO_URL (default branch)..."
    git clone "$REPO_URL"
  fi

  cd "$TARGET_DIR"

  # Build setup
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

  echo "[INFO] Build complete."
  echo "[INFO] Cleaning up temporary files..."
  rm -rf "$SCRIPT_DIR"

  echo "[INFO] Done. Project is located at $TARGET_DIR"
  exit 0
fi

# Option 4: Remove temporary script files
if [[ "$CHOICE" == "4" ]]; then
  echo "[INFO] Searching for temporary copies of $SCRIPT_NAME..."
  FOUND_DIRS=()

  # Search known temp locations
  for dir in /tmp /private/var/folders; do
    if [[ -d "$dir" ]]; then
      while IFS= read -r -d '' file; do
        if grep -q "$SCRIPT_NAME" <<< "$file"; then
          parent_dir=$(dirname "$file")
          FOUND_DIRS+=("$parent_dir")
        fi
      done < <(find "$dir" -type f -name "$SCRIPT_NAME" -print0 2>/dev/null)
    fi
  done

  if [[ "${#FOUND_DIRS[@]}" -eq 0 ]]; then
    echo "No temporary script files found."
    exit 0
  fi

  echo "Found the following temporary script directories:"
  for i in "${!FOUND_DIRS[@]}"; do
    echo "$((i + 1)). ${FOUND_DIRS[$i]}"
  done

  echo
  echo "1. Delete all"
  echo "2. Delete individually"
  read -p "Choose an option [1-2]: " DELETE_OPTION

  if [[ "$DELETE_OPTION" == "1" ]]; then
    for dir in "${FOUND_DIRS[@]}"; do
      echo "Deleting $dir"
      rm -rf "$dir"
    done
    echo "All temporary script directories removed."
  elif [[ "$DELETE_OPTION" == "2" ]]; then
    for dir in "${FOUND_DIRS[@]}"; do
      read -p "Delete $dir? [y/N]: " CONFIRM
      if [[ "$CONFIRM" == "y" || "$CONFIRM" == "Y" ]]; then
        rm -rf "$dir"
        echo "Deleted $dir"
      fi
    done
  else
    echo "Invalid selection. No files deleted."
  fi

  exit 0
fi

# Option 5: Exit
if [[ "$CHOICE" == "5" ]]; then
  echo "Goodbye!"
  exit 0
fi

# Placeholder options
if [[ "$CHOICE" == "2" || "$CHOICE" == "3" ]]; then
  echo "This option is a placeholder. Nothing to do yet."
  exit 0
fi
