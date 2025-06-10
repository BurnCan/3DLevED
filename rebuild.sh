#!/bin/bash
set -x  # Enable debugging: print each command before executing

set -e

# Detect where the script is running from
SCRIPT_PATH=$(realpath "$0")
SCRIPT_NAME=$(basename "$SCRIPT_PATH")

# Create a temporary directory
TEMP_DIR=$(mktemp -d)

# Copy the script to the temporary directory
cp "$SCRIPT_PATH" "$TEMP_DIR/"

# Change to the temporary directory
cd "$TEMP_DIR"

# Execute the script from the temp directory
exec "$TEMP_DIR/$SCRIPT_NAME"

# Below this point will not be executed until the script is re-run from the temp directory

# Ask for user input
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

# Save HOME directory as the root destination
HOME_DIR="$HOME"

# Remove the original repo directory from HOME if it exists
TARGET_DIR="$HOME_DIR/$REPO_NAME"
if [ -d "$TARGET_DIR" ]; then
    echo "Removing existing directory: $TARGET_DIR"
    rm -rf "$TARGET_DIR"
fi

# Clone the repository directly into HOME
echo "Cloning branch '$BRANCH' of $REPO_URL into $HOME_DIR..."
git clone --branch "$BRANCH" --single-branch "$REPO_URL" "$HOME_DIR/$REPO_NAME"

# Enter the project and build
cd "$HOME_DIR/$REPO_NAME"
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

echo "Build complete in $TARGET_DIR/build."

# Cleanup
echo "Removing temporary directory: $TEMP_DIR"
rm -rf "$TEMP_DIR"

echo "Done. Project is located at: $TARGET_DIR"
