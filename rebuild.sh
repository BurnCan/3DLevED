#!/bin/bash
set -e
set -x

# Absolute path to the script
SCRIPT_PATH=$(realpath "$0")
SCRIPT_NAME=$(basename "$SCRIPT_PATH")

# Prevent infinite recursion by only copying if not in /tmp
if [[ "$SCRIPT_PATH" != /tmp/* ]]; then
    TEMP_DIR=$(mktemp -d)
    cp "$SCRIPT_PATH" "$TEMP_DIR/"
    cd "$TEMP_DIR"
    exec "$TEMP_DIR/$SCRIPT_NAME"
fi

# Move to home directory
cd ~

# Prompt for repo URL
read -p "Enter the GitHub repo URL [https://github.com/yourusername/your-repo.git]: " REPO_URL
REPO_URL="${REPO_URL:-https://github.com/yourusername/your-repo.git}"

# Extract repo name from URL
REPO_NAME=$(basename -s .git "$REPO_URL")

# Prompt for branch name (default to main)
read -p "Enter the branch to clone [main]: " BRANCH
BRANCH="${BRANCH:-main}"

# Remove existing repo directory if it exists
if [ -d "$REPO_NAME" ]; then
    echo "Removing existing directory $REPO_NAME..."
    rm -rf "$REPO_NAME"
fi

# Clone the specified branch into home directory
git clone --branch "$BRANCH" --single-branch "$REPO_URL"

# Build
cd "$REPO_NAME"
mkdir build
cd build

UNAME=$(uname)

if [[ "$UNAME" == "Darwin" ]]; then
    cmake ..
    make -j4
elif [[ "$UNAME" == "Linux" ]]; then
    cmake ..
    make -j4
elif [[ "$UNAME" =~ MINGW64_NT|MSYS_NT ]]; then
    cmake -G "MinGW Makefiles" ..
    mingw32-make -j4
else
    echo "Unsupported platform: $UNAME"
    exit 1
fi

# Done
echo "Build completed for $REPO_NAME"

# Cleanup
cd ~
rm -rf "$(dirname "$SCRIPT_PATH")"
