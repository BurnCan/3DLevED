#!/bin/bash
set -e

# Get absolute path to this script
if command -v realpath &> /dev/null; then
  SCRIPT_PATH=$(realpath "$0")
else
  SCRIPT_PATH="$(cd "$(dirname "$0")" && pwd)/$(basename "$0")"
fi

SCRIPT_NAME=$(basename "$SCRIPT_PATH")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# Avoid infinite self-copying by only doing this if not already running in a temp folder
if [[ "$SCRIPT_DIR" != /tmp/* && "$SCRIPT_DIR" != /private/var/folders/* ]]; then
  TEMP_DIR=$(mktemp -d)
  cp "$SCRIPT_PATH" "$TEMP_DIR/"
  cd "$TEMP_DIR"
  exec "$TEMP_DIR/$SCRIPT_NAME"
fi

#!/bin/bash
set -e

# Get absolute path to this script
if command -v realpath &> /dev/null; then
  SCRIPT_PATH=$(realpath "$0")
else
  SCRIPT_PATH="$(cd "$(dirname "$0")" && pwd)/$(basename "$0")"
fi

SCRIPT_NAME=$(basename "$SCRIPT_PATH")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# Avoid infinite self-copying
if [[ "$SCRIPT_DIR" != /tmp/* && "$SCRIPT_DIR" != /private/var/folders/* ]]; then
  TEMP_DIR=$(mktemp -d)
  cp "$SCRIPT_PATH" "$TEMP_DIR/"
  cd "$TEMP_DIR"
  exec "$TEMP_DIR/$SCRIPT_NAME"
fi

# === Prompt user for action ===
echo "What would you like to do?"
echo "1) Clean install from repository"
echo "2) [Placeholder] Update existing build"
echo "3) [Placeholder] Open project folder"
echo "4) [Placeholder] Exit"

read -rp "Enter your choice [1-4]: " CHOICE

case "$CHOICE" in
  1)
    echo "[INFO] Starting clean install from repository..."

    # Prompt for repo URL
    read -p "Enter the git repository URL (e.g., https://github.com/yourusername/your-repo.git): " REPO_URL
    REPO_URL=${REPO_URL:-"https://github.com/yourusername/your-repo.git"}

    # Prompt for branch name (optional)
    read -p "Enter the branch name to clone (leave blank for default): " BRANCH_NAME

    # Extract repo name from URL
    REPO_NAME=$(basename -s .git "$REPO_URL")

    # Clone to $HOME/repo_name (clean)
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
    ;;

  2)
    echo "[TODO] Option 2: Update existing build (placeholder)"
    ;;

  3)
    echo "[TODO] Option 3: Open project folder (placeholder)"
    ;;

  4)
    echo "Exiting..."
    exit 0
    ;;

  *)
    echo "Invalid choice. Exiting."
    exit 1
    ;;
esac

