echo
read -p "Do you want to run the application now? [y/N]: " RUN_APP
if [[ "$RUN_APP" =~ ^[Yy]$ ]]; then
  APP_DIR="$TARGET_DIR/build/bin"
  
  if [[ "$OS_NAME" == "Darwin" ]]; then
    # For macOS, app is inside a .app bundle
    APP_EXEC="$APP_DIR/$REPO_NAME.app/Contents/MacOS/$REPO_NAME"
    if [[ -x "$APP_EXEC" ]]; then
      cd "$APP_DIR/$REPO_NAME.app/Contents/MacOS"
      echo "[INFO] Launching macOS application..."
      "$APP_EXEC"
    else
      echo "[ERROR] macOS application binary not found or not executable: $APP_EXEC"
    fi
  elif [[ "$OS_NAME" == "Linux" ]]; then
    # For Linux, directly running the app from bin/
    APP_EXEC="$APP_DIR/$REPO_NAME"
    if [[ -x "$APP_EXEC" ]]; then
      cd "$APP_DIR"
      echo "[INFO] Launching Linux application..."
      "$APP_EXEC"
    else
      echo "[ERROR] Linux application binary not found or not executable: $APP_EXEC"
    fi
  elif [[ "$OS_NAME" == "CYGWIN"* || "$OS_NAME" == "MINGW"* ]]; then
    # For Windows (MinGW)
    APP_EXEC="$APP_DIR/$REPO_NAME.exe"
    if [[ -x "$APP_EXEC" ]]; then
      cd "$APP_DIR"
      echo "[INFO] Launching Windows application..."
      "$APP_EXEC"
    else
      echo "[ERROR] Windows application binary not found or not executable: $APP_EXEC"
    fi
  else
    echo "[ERROR] Unsupported OS: $OS_NAME. Please manually run the application."
  fi
else
  cd ~
  echo "[INFO] Returned to home directory."
fi

