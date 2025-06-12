#!/bin/bash
set -e

SCRIPT_PATH=$(realpath "$0")
SCRIPT_NAME=$(basename "$SCRIPT_PATH")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# === Option 1: Clean install ===
clean_install() {
  echo "[INFO] Preparing for clean install..."

  if [[ "$SCRIPT_DIR" != /tmp/* && "$SCRIPT_DIR" != /private/var/folders/* ]]; then
    TEMP_DIR=$(mktemp -d)
    cp "$SCRIPT_PATH" "$TEMP_DIR/"
    cd "$TEMP_DIR"
    echo "[INFO] Copied script to temp directory: $TEMP_DIR"
    exec "$TEMP_DIR/$SCRIPT_NAME"
  fi

  echo "[INFO] Running clean install from temp directory..."

  read -p "Enter the git repository URL (e.g., https://github.com/yourusername/your-repo.git): " REPO_URL
  REPO_URL=${REPO_URL:-"https://github.com/yourusername/your-repo.git"}

  read -p "Enter the branch name to clone (leave blank for default): " BRANCH_NAME

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

  echo "[INFO] Build complete."
  echo "[INFO] Cleaning up temporary files..."
  rm -rf "$SCRIPT_DIR"
  echo "[INFO] Done. Project is located at $TARGET_DIR"
}

# === Option 4: Remove temp scripts ===
remove_temp_scripts() {
  echo "[INFO] Searching for temporary copies of $SCRIPT_NAME..."

  FOUND_DIRS=()
  SEARCH_PATHS=("/tmp" "/private/var/folders")

  for LOC in "${SEARCH_PATHS[@]}"; do
    while IFS= read -r -d '' FILE; do
      DIR_FOUND=$(dirname "$FILE")
      FOUND_DIRS+=("$DIR_FOUND")
    done < <(find "$LOC" -type f -name "$SCRIPT_NAME" -print0 2>/dev/null)
  done

  if [[ ${#FOUND_DIRS[@]} -eq 0 ]]; then
    echo "No temporary script files found."
    return
  fi

  echo
  echo "Found the following directories containing '$SCRIPT_NAME':"
  for i in "${!FOUND_DIRS[@]}"; do
    echo "[$i] ${FOUND_DIRS[$i]}"
  done
  echo

  echo "Choose deletion method:"
  echo "1. Delete all found directories"
  echo "2. Manually delete selected directories"
  read -p "Enter option [1-2]: " DELETE_OPTION

  if [[ "$DELETE_OPTION" == "1" ]]; then
    for DIR in "${FOUND_DIRS[@]}"; do
      echo "[INFO] Deleting: $DIR"
      rm -rf "$DIR"
    done
    echo "[INFO] All temporary script directories deleted."
  elif [[ "$DELETE_OPTION" == "2" ]]; then
    for i in "${!FOUND_DIRS[@]}"; do
      read -p "Delete ${FOUND_DIRS[$i]}? [y/n]: " CONFIRM_ONE
      if [[ "$CONFIRM_ONE" == "y" ]]; then
        echo "[INFO] Deleting: ${FOUND_DIRS[$i]}"
        rm -rf "${FOUND_DIRS[$i]}"
      else
        echo "[SKIPPED] ${FOUND_DIRS[$i]}"
      fi
    done
    echo "[INFO] Manual cleanup complete."
  else
    echo "[WARN] Invalid option. Aborting cleanup."
  fi
}

# === Main Menu ===
if [[ "$SCRIPT_DIR" == /tmp/* || "$SCRIPT_DIR" == /private/var/folders/* ]]; then
  clean_install
  exit 0
fi

while true; do
  echo "========== 3DLevED Project Setup =========="
  echo "1. Clean install from repository"
  echo "2. Placeholder Option 2"
  echo "3. Placeholder Option 3"
  echo "4. Remove temporary script files"
  echo "5. Exit"
  echo "==========================================="
  read -p "Choose an option [1-5]: " CHOICE
  echo

  case "$CHOICE" in
    1) clean_install ;;
    2) echo "[TODO] Placeholder Option 2 not implemented yet." ;;
    3) echo "[TODO] Placeholder Option 3 not implemented yet." ;;
    4) remove_temp_scripts ;;
    5) echo "Goodbye."; exit 0 ;;
    *) echo "Invalid choice. Please enter 1-5." ;;
  esac

  echo
done
