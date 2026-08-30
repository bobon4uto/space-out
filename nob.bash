#!/bin/bash
set -ex

function build() {

  mkdir -p build/obj
  mkdir -p build/web
  local CC="cc"
  local OUTPUT="build/game"
  local CFLAGS="-Wall -std=c23 -D_DEFAULT_SOURCE -Os -D_DEBUG"
  local RAYLIB_PATH="./thirdparty/raylib"
  local EXTRAFLAGS=""
  local LDFLAGS="-I. -I$RAYLIB_PATH/src -I$RAYLIB_PATH/src/external -L$RAYLIB_PATH/src -L. -lraylib -lm -lX11"

  if [[ "$1" == "web" ]]; then
    rm -rf web
    mkdir web

    python templates/template.py templates/shell_template.html templates/shell_variables.txt src/shell.html
    local CC="emcc"
    local OUTPUT="build/web/index.html"
    local LDFLAGS="-I. -I$RAYLIB_PATH/src -I$RAYLIB_PATH/src/external -L. -L$RAYLIB_PATH/src -lraylib.web  "
    local EXTRAFLAGS="-DPLATFORM_WEB -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 -s EXPORTED_RUNTIME_METHODS=ccall \
      --preload-file ./assets --shell-file src/shell.html"
  fi

  $CC -c src/main.c -o build/obj/main.o $CFLAGS
  $CC -c src/one.c -o build/obj/one.o $CFLAGS

  $CC build/obj/main.o build/obj/one.o -o $OUTPUT $CFLAGS $LDFLAGS $EXTRAFLAGS
  if [[ "$2" == "run" ]]; then
    $OUTPUT
  fi
}

build native run
