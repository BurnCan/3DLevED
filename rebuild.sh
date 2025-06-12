#!/bin/bash
set -e

# Resolve absolute path and script info
SCRIPT_PATH=$(realpath "$0")
SCRIPT_NAME=$(basename "$SCRIPT_PATH")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# Determine if running from a temporary location
IS_TEMP_SCRIPT=false
if [[ "$SCRIPT_DIR" == /tmp/* || "$SCRIPT_DIR" == /private/var/folders/* ]]; then
  IS_TEMP_SCRIPT=true
fi

# Auto-select Option 1 if already in a temp dir
if $IS_TEMP_SCRIPT; then
  OPTION=1
else
  # Interactive menu
  echo "========== 3DLevED Project Setup =========="
  echo "1. Clean install from repository"
  echo "2. Placeholder Option 2"
  echo "3. Placeholder Option 3"
  echo "4. Remove temporary script files"
  echo "5. Exit"
  echo "==========================================="
  read -rp "Choose an option [1-5]: " OPTION
fi

case "$OPTION" in
  1)
    # If not running from temp dir, move to temp and exec
    if ! $IS_TEMP_SCRIPT; then
      TEMP_DIR=$(mktemp -d)
      cp "$SCRIPT_PATH" "$TEMP_DIR/"
      cd "$TEMP_DIR"
      exec "$TEMP_DIR/$SCRIPT_NAME"
    fi

    # Prompt for repo URL
    read -p "Enter the git repository URL (e.g., https://github.com/yourusername/your-repo.git): " REPO_URL
    REPO_URL=${REPO_URL:-"https://github.com/yourusername/your-repo.git"}

    # Prompt for branch name (optional)
    read -p "Enter the branch name to clone (leave blank for default): " BRANCH_NAME

    # Extract repo name from URL
    REPO_NAME=$(basename -s .git "$REPO_URL")
    TARGET_DIR="$HOME/$REPO_NAME"

    # Go to home and remove existing repo directory
    cd "$HOME"
    if [ -d "$TARGET_DIR" ]; then
      echo "[INFO] Removing existing directory $TARGET_DIR"
      rm -rf "$TARGET_DIR"
    fi

    # Clone repo into home directory
    if [[ -n "$BRANCH_NAME" ]]; then
      echo "[INFO] Cloning $REPO_URL (branch: $BRANCH_NAME)..."
      git clone --branch "$BRANCH_NAME" --single-branch "$REPO_URL"
    else
      echo "[INFO] Cloning $REPO_URL (default branch)..."
      git clone "$REPO_URL"
    fi

    cd "$TARGET_DIR"

    # Create build directory
    mkdir -p build
    cd build

    # Detect platform and build accordingly
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
    echo "[INFO] Done. Project is located at $TARGET_DIR"

    # Clean up temp directory
    echo "[INFO] Cleaning up temporary files..."
    rm -rf "$SCRIPT_DIR"
    ;;

  2)
    echo "[TODO] Option 2 placeholder."
    ;;

  3)
    echo "[TODO] Option 3 placeholder."
    ;;

  4)
    echo "[INFO] Searching for temporary script files..."
    FOUND_DIRS=()
    TEMP_LOCATIONS=("/tmp" "/private/var/folders")

    for LOC in "${TEMP_LOCATIONS[@]}"; do
      if [ -d "$LOC" ]; then
        while IFS= read -r -d '' file; do
          DIR_FOUND=$(dirname "$file")
          FOUND_DIRS+=("$DIR_FOUND")
        done < <(find "$LOC" -type f -name "$SCRIPT_NAME" -print0 2>/dev/null)
      fi
    done

    if [ ${#FOUND_DIRS[@]} -eq 0 ]; then
      echo "[INFO] No temporary script files found."
    else
      echo "[INFO] Found temporary script files in:"
      for i in "${!FOUND_DIRS[@]}"; do
        echo "$((i+1)). ${FOUND_DIRS[$i]}"
      done

      echo "Choose an action:"
      echo "[1] Delete All"
      echo "[2] Choose individually"
      echo "[3] Cancel"
      read -rp "> " DELETE_OPTION

      case "$DELETE_OPTION" in
        1)
          for DIR in "${FOUND_DIRS[@]}"; do
            echo "[INFO] Deleting $DIR"
            rm -rf "$DIR"
          done
          echo "[INFO] All temporary script files deleted."
          ;;
        2)
          for DIR in "${FOUND_DIRS[@]}"; do
            read -rp "Delete $DIR? [y/n]: " CONFIRM
            if [[ "$CONFIRM" == "y" || "$CONFIRM" == "Y" ]]; then
              rm -rf "$DIR"
              echo "[INFO] Deleted $DIR"
            else
              echo "[INFO] Skipped $DIR"
            fi
          done
          ;;
        *)
          echo "[INFO] Canceled."
          ;;
      esac
    fi
    ;;

  5)
    echo "Exiting."
    exit 0
    ;;

  *)
    echo "Invalid option. Exiting."
    exit 1
    ;;
esac
