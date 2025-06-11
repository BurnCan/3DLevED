#!/bin/bash
set -e

# Resolve the real path of the script
SCRIPT_PATH="$(realpath "$0")"
SCRIPT_DIR="$(dirname "$SCRIPT_PATH")"
SCRIPT_NAME="$(basename "$SCRIPT_PATH")"

# Safeguard: if not already in /tmp or macOS temp path, re-run from temp
if [[ "$SCRIPT_DIR" != /tmp/* && "$SCRIPT_DIR" != /private/var/folders/* ]]; then
  TEMP_DIR=$(mktemp -d)
  cp "$SCRIPT_PATH" "$TEMP_DIR/"
  cd "$TEMP_DIR"
  exec "$TEMP_DIR/$SCRIPT_NAME"
fi

# Ask for repository URL
read -p "Enter the Git repository URL (e.g., https://github.com/yourusername/your-repo.git): " REPO_URL
REPO_URL=${REPO_URL:-"https://github.com/yourusername/your-repo.git"}

# Ask for branch name
read -p "Enter branch name to clone (leave empty for default): " BRANCH

# Extract repo name
REPO_NAME=$(basename -s .git "$REPO_URL")
CLONE_DIR="$HOME/$REPO_NAME"

# Remove old directory
echo "[INFO] Removing existing directory at $CLONE_DIR"
rm -rf "$CLONE_DIR"

# Clone repo
echo "[INFO] Cloning repository..."
if [[ -n "$BRANCH" ]]; then
  git clone --branch "$BRANCH" --single-branch "$REPO_URL" "$CLONE_DIR"
else
  git clone "$REPO_URL" "$CLONE_DIR"
fi

# Build
cd "$CLONE_DIR"
mkdir -p build
cd build

echo "[INFO] Running CMake and build..."

UNAME_OUT="$(uname -s)"
case "${UNAME_OUT}" in
    Linux*|Darwin*)
        cmake ..
        make -j4
        ;;
    MINGW*|MSYS*|CYGWIN*)
        cmake -G "MinGW Makefiles" ..
        mingw32-make -j4
        ;;
    *)
        echo "[ERROR] Unsupported platform: ${UNAME_OUT}"
        exit 1
        ;;
esac

# Optional copy (skip if same directory)
SOURCE_DIR="$(pwd)/.."
DEST_DIR="$HOME/$REPO_NAME"

if [ "$SOURCE_DIR" != "$DEST_DIR" ]; then
  echo "[INFO] Copying build output to $DEST_DIR"
  cp -r "$SOURCE_DIR" "$DEST_DIR"
else
  echo "[INFO] Skipping copy: source and destination are the same ($SOURCE_DIR)"
fi

echo "[SUCCESS] Build completed."
