#!/usr/bin/env bash
set -e

IMAGE=molviewer

# 1. Stop early if there's no display to draw on
if [ -z "$DISPLAY" ]; then
  echo "No display found (DISPLAY is empty)."
  exit 1
fi

# 2. Build the image if it doesn't exist yet
if ! docker image inspect "$IMAGE" > /dev/null 2>&1; then
  docker build -t "$IMAGE" "$(dirname "$0")"
fi

# 3. Display options shared by Linux and WSL
DISPLAY_ARGS=(-e DISPLAY="$DISPLAY" -v /tmp/.X11-unix:/tmp/.X11-unix)

# 4. Add the WSL-specific or Linux-specific part
if uname -r | grep -qi microsoft; then
  DISPLAY_ARGS+=(-v /mnt/wslg:/mnt/wslg)
else
  xhost +local:docker > /dev/null
fi

# 5. Run it, passing along any arguments (e.g. a molecule file)
docker run --rm "${DISPLAY_ARGS[@]}" "$IMAGE" "$@"
