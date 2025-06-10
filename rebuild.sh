#!/bin/bash

set -e

echo "Enter the Git repository URL (e.g., https://github.com/yourusername/your-repo.git):"
read -r REPO_URL

if [ -z "$REPO_URL" ]; then
    echo "Error: No URL provided."
    exit 1
fi

echo "Enter the branch to clone (leave blank for 'main'):"
read -r BRANCH

# Default to main if blank
if [ -z "$BRANCH" ]; then
    BRANCH="main"
fi

# Extract repo name from URL
REPO_NAME=$(basename -s .git "$REPO_URL")

# Save original directory
ORIGINAL_DIR=$(pwd)

# Create temporary folder and copy this script
TEMP_DIR=$(mktemp -d)
SCRIPT_NAME=$(basename "$0")
cp "$0" "$TEMP_DIR/"

# Clone target path
CLONE_DIR="$ORIGINAL_DIR/$REPO_NAME"

# Clean existing repo directory
if [ -d "$CLONE_DIR" ]; then
    echo "Removing existing directory: $CLONE_DIR"
    rm -rf "$CLONE_DIR"
fi

# Clone the specific branch
echo "Cloning branch '$BRANCH' of $REPO_URL into $CLONE_DIR..."
git clone --branch "$BRANCH" --single-branch "$REPO_URL" "$CLONE_DIR"

# Move into cloned repo
cd "$CLONE_DIR" || exit 1

# Build
echo "Starting build..."
mkdir -p build
cd build

OS_NAME="$(uname)"

if [[ "$OS_NAME" == "Darwin" ]]; then
    echo "Detected macOS."
    cmake ..
    make -j4
elif [[ "$OS_NAME" == "Linux" ]]; then
    echo "Detected Linux."
    cmake ..
    make -j4
elif [[ "$OS_NAME" =~ MINGW64_NT|MSYS_NT|CYGWIN_NT ]]; then
    echo "Detected Windows (MSYS2/MINGW64)."
    cmake -G "MinGW Makefiles" ..
    mingw32-make -j4
else
    echo "Unsupported OS: $OS_NAME"
    exit 1
fi

# Copy built repo to original directory
cd "$ORIGINAL_DIR"
cp -r "$CLONE_DIR" .

# Clean up
echo "Build files copied to $ORIGINAL_DIR/$REPO_NAME."
echo "Removing temporary directory: $TEMP_DIR"
rm -rf "$TEMP_DIR"

echo "Done."

