#!/usr/bin/env bash

set -e

# Prompt for repo URL
read -p "Enter the repository URL (e.g., https://github.com/yourusername/your-repo.git): " REPO_URL

# Extract repo name
REPO_NAME=$(basename -s .git "$REPO_URL")

# Prompt for branch name (optional)
read -p "Enter the branch name (press Enter for default): " BRANCH
BRANCH_OPTION=""
if [[ -n "$BRANCH" ]]; then
    BRANCH_OPTION="--branch $BRANCH --single-branch"
fi

# Create a temporary working directory
TEMP_DIR=$(mktemp -d -t repo_clone_XXXXXX)
echo "Cloning into temporary directory: $TEMP_DIR"

# Clone the repository
git clone $BRANCH_OPTION "$REPO_URL" "$TEMP_DIR/$REPO_NAME"
cd "$TEMP_DIR/$REPO_NAME"

# Make and enter build directory
mkdir -p build
cd build

# Detect platform
UNAME=$(uname)
if [[ "$UNAME" == "Darwin" ]]; then
    echo "Building for macOS..."
    cmake ..
    make -j4
elif [[ "$UNAME" == "Linux" ]]; then
    echo "Building for Linux..."
    cmake ..
    make -j4
elif [[ "$UNAME" =~ "MINGW" || "$UNAME" =~ "MSYS" ]]; then
    echo "Building for Windows (MSYS2)..."
    cmake -G "MinGW Makefiles" ..
    mingw32-make -j4
else
    echo "Unsupported platform: $UNAME"
    exit 1
fi

# Go back to original directory
cd "$TEMP_DIR"

# Copy build output to root directory using repo name
DEST="$PWD/../../$REPO_NAME"
rm -rf "$DEST"
cp -r "$REPO_NAME" "$DEST"
echo "Build files copied to $DEST."

# Clean up
cd ../..
rm -rf "$TEMP_DIR"
echo "Removed $TEMP_DIR."
