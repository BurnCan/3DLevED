#!/usr/bin/env bash
set -e

# Resolve the full path of the script
SCRIPT_PATH=$(realpath "$0")
SCRIPT_NAME=$(basename "$SCRIPT_PATH")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# Check if we're already in a temporary directory (Linux or macOS)
if [[ "$SCRIPT_DIR" != /tmp/* && "$SCRIPT_DIR" != /private/var/folders/* ]]; then
    # Create a new temporary directory
    TEMP_DIR=$(mktemp -d)

    # Copy only the script to the temp dir and re-run from there
    cp "$SCRIPT_PATH" "$TEMP_DIR/"
    cd "$TEMP_DIR"
    exec "$TEMP_DIR/$SCRIPT_NAME"
fi

# From here on, we're running inside a temp dir
echo "[INFO] Running from temp directory: $SCRIPT_DIR"

# Prompt for the repository URL
read -p "Enter the Git repository URL [example: https://github.com/yourusername/your-repo.git]: " REPO_URL
REPO_URL=${REPO_URL:-"https://github.com/yourusername/your-repo.git"}

# Extract repo name from URL
REPO_NAME=$(basename -s .git "$REPO_URL")

# Prompt for branch (optional)
read -p "Enter the branch name to clone [default: main]: " BRANCH
BRANCH=${BRANCH:-main}

# Navigate to home directory or root project directory
cd ~

# Remove existing project directory if it exists
if [[ -d "$REPO_NAME" ]]; then
    echo "[INFO] Removing existing directory: $REPO_NAME"
    rm -rf "$REPO_NAME"
fi

# Clone the desired branch
echo "[INFO] Cloning $REPO_URL (branch: $BRANCH)"
git clone --branch "$BRANCH" --single-branch "$REPO_URL"

# Enter the repo directory
cd "$REPO_NAME"

# Create and enter build directory
mkdir -p build
cd build

# Detect platform
OS="$(uname -s)"
if [[ "$OS" == "MINGW"* || "$OS" == "MSYS"* || "$OS" == "CYGWIN"* ]]; then
    echo "[INFO] Detected Windows environment (MSYS2). Using MinGW Makefiles."
    cmake -G "MinGW Makefiles" ..
    mingw32-make -j4
else
    echo "[INFO] Detected Unix-like environment. Using default Makefiles."
    cmake ..
    make -j4
fi

# Return to parent of build directory (repo root)
cd ..

# Copy build artifacts to target location
echo "[INFO] Copying build output to ~/$REPO_NAME"
cp -r . ~/"$REPO_NAME"

# Cleanup temp directory
echo "[INFO] Cleaning up temporary directory: $SCRIPT_DIR"
rm -rf "$SCRIPT_DIR"

echo "[✅] Build completed and copied to ~/$REPO_NAME"
